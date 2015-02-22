// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include <algorithm>
#include "core/exception.hpp"
#include "core/container/intrusive_list.hpp"
#include "core/math/util.hpp"
#include "core/memory/pool_allocator.hpp"
#include "core/memory/memory_region_free_list.hpp"

holo::pool_allocator::pool_allocator
	(holo::memory_region_free_list* free_list, std::size_t object_size) :
		memory_region_list_head(nullptr),
		memory_region_list_tail(nullptr),
		memory_region_free_list(free_list),
		// Increase the object size to fit a free_pool_node. This is necessary
		// because when an object is returned to the pool, its memory is re-used
		// to store a a portion of a free_pool_node, and thus must be at least as
		// large as the portion of the free_pool_node stored.
		object_size(std::max(object_size, sizeof(free_pool_node) - sizeof(allocation_header)))
{
	holo_assert(free_list != nullptr);
}

holo::pool_allocator::~pool_allocator()
{
	auto current_iterator = intrusive_list::make_iterator(memory_region_list_head);
	auto end_iterator = intrusive_list::end<memory_region_header>();
	
	// Return the memory regions to the free list.
	while (current_iterator != end_iterator)
	{
		// Any references to the header are forfeit once it is pushed to the free
		// list. We must fetch the next iterator before then and cache the current
		// iterator's value.
		auto current = *current_iterator++;

		memory_region_free_list->push(current);
	}
}

void* holo::pool_allocator::allocate(std::size_t size, std::size_t)
{
	// Sanity check.
	if (size > object_size)
	{
		// The allocation cannot succeed. Allocated blocks are assumed to be
		// 'object_size' bytes large.
		push_exception(holo::exception::invalid_argument);
		
		return nullptr;
	}

	pool_node* node = get_first_free_node(memory_region_list_head);
	if (node == nullptr)
	{
		// There is no free space in any of the currently reserved regions, so
		// request a new memory region from the pool allocator.
		memory_region_header* new_memory_region = request_empty_memory_region();

		if (new_memory_region == nullptr)
		{
			// Allocation failed. Propagate the exception and return.
			return nullptr;
		}

		// This should always succeed: the new memory was allocated, and thus there
		// should be at least one free node.
		node = get_first_free_node(new_memory_region);
	}

	free_pool_node* next_free_data = nullptr;
	if (node->size > 1)
	{
		// The node must be broken up into a small and a large chunk.
		//
		// Since free_pool_node objects are stored in the public span in a linear
		// order, we can just increment by default_align + object_size (keep in
		// mind that the allocation header is at most default_align bytes large)
		// and put the next free node there. Essentially, the public span is just
		// an array with (default_align + object_size) sized elements.
		next_free_data =
			(free_pool_node*)((char*)node->free_data + default_align + object_size);

		// The pool node list can also be considered a sort-of 'sparse' array, where
		// cells are only complete when necessary. Therefore, to retrieve the next
		// pool_node object, just increment the current pool_node pointer.
		pool_node* next_node = node + 1;

		// Initialize and insert the next node.
		next_node->free_data = next_free_data;
		next_node->size = node->size - 1;

		intrusive_list::insert_after(next_node, node);

		// Initialize and insert the free data.
		// This will also update the allocation header.
		next_free_data->memory_region = node->free_data->memory_region;
		next_free_data->current = next_node;

		intrusive_list::insert_after(next_free_data, node->free_data);

		// Finally, update the node.
		node->size = 1;
	}

	// Keep in mind this was the first free node found, thus its 'previous'
	// pointer is NULL; so update the tail to the next node, and then remove
	// the node.
	node->free_data->memory_region->free_pool_node_list = next_free_data;

	// Decrease the number of free nodes.
	--node->free_data->memory_region->free_nodes;
	intrusive_list::remove(node->free_data);

	// We don't have to update the allocation_header. By design, the fields of
	// the allocation header map to the first fields of the free_pool_node
	// object for this reason.
	//
	// 'free_data' is already adjusted by alignment requirements (so that the
	// allocation header will be placed right before the returned pointer).
	//
	// So just return the pointer, offset by sizeof(allocation_header).
	void* pointer = ((char*)node->free_data) + sizeof(allocation_header);

	// Mark the node as free by delinking it from the free_data.
	node->free_data = nullptr;

	// Memory allocation is good to go! Return.
	return pointer;
}

void holo::pool_allocator::deallocate(void* pointer)
{
	allocation_header* header = (allocation_header*)pointer - 1;
	pool_node*  node = header->node;

	// Setup the node's free data.
	make_free_node(header);

	// Check if free_nodes == memory_region->size. If so, this region should be
	// returned back to the free list unless it's the only memory region in the
	// local pool list.
	memory_region_header* memory_region = node->free_data->memory_region;
	++memory_region->free_nodes;

	// If memory_region->free_nodes > memory_region->size, we have
	// (at some point) deallocated a stale pointer. Better late than never in
	// catching the error!
	holo_assert(memory_region->free_nodes <= memory_region->size);

	if (memory_region->free_nodes == memory_region->size
		&& memory_region_list_head != memory_region_list_tail)
	{
		// Update the head or tail if necessary.
		if (memory_region_list_head == memory_region)
		{
			memory_region_list_head = memory_region->next;
		}
		else if (memory_region_list_tail == memory_region)
		{
			memory_region_list_tail = memory_region->previous;
		}

		// Remove the node from the list.
		intrusive_list::remove(memory_region);

		// ...and tell the region list it's free for use.
		memory_region_free_list->push(memory_region);
	}
}

std::size_t holo::pool_allocator::get_object_size() const
{
	return object_size;
}

holo::pool_allocator::pool_node* holo::pool_allocator::get_first_free_node(memory_region_header* header)
{
	// Find the first memory region, in order of age, that has a free node,
	// starting with the provided header.
	auto current_memory_region_iter = intrusive_list::make_iterator(header);
	auto end_memory_region_iter = intrusive_list::end<memory_region_header>();

	while (current_memory_region_iter != end_memory_region_iter)
	{
		memory_region_header* current_memory_region_header = *current_memory_region_iter;

		if (current_memory_region_header->free_pool_node_list != nullptr)
		{
			free_pool_node* free_node = current_memory_region_header->free_pool_node_list;

			// The free node is guaranteed to store at least enough space for one
			// allocation (with respect to 'object_size'), so just return it.
			pool_node* node = free_node->current;
			return node;
		}

		current_memory_region_iter++;
	}

	// No free space in any of the regions. Signal the caller by returning NULL.
	return nullptr;
}

holo::pool_allocator::memory_region_header* holo::pool_allocator::request_empty_memory_region()
{
	void* requested_region = memory_region_free_list->pop();

	// Uh-oh, we're out of memory.
	if (requested_region == nullptr)
	{
		// Just propogate holo::exception::out_of_memory, or whatever other error
		// may have caused the free list to fail.
		return nullptr;
	}

	// The header of a region is stored at the beginning of the region.
	memory_region_header* requested_region_header =
		(memory_region_header*)requested_region;

	// Prepare the region.
	requested_region_header->next = nullptr;
	requested_region_header->previous = nullptr;

	// Calculate the header size to create the first pool node. This node is
	// aligned on a safe boundary.
	std::size_t header_size =
		get_pointer_distance(align_pointer(requested_region_header + 1, alignof(pool_node)), requested_region);
	pool_node* node = (pool_node*)((char*)requested_region + header_size);

	// Setup the memory region for use with the allocator.
	//
	// 'next' and 'previous' are relative to the nodes in the region, not the pool.
	node->next = nullptr;
	node->previous = nullptr;

	std::size_t available_region_size =
		memory_region_free_list->get_memory_region_size() - header_size - default_align - 1;
	std::size_t allocated_node_size = sizeof(pool_node) + sizeof(allocation_header) + object_size;

	// 'size' is in multiples of 'object_size', not bytes; thus, we find the
	// maximum amount of blocks that can be stored in this region and use that as
	// the 'size' value.
	node->size = available_region_size / allocated_node_size;

	// If the node size is zero, then a fatal error has occurred in the
	// creation of the pool allocator or the free list: it means the object_size
	// and extra bookkeeping data are too large for a region returned from the
	// free list--too large to even store one block.
	holo_assert(node->size > 0);

	// Prepare the 'free_data' member.
	//
	// free_pool_node objects are stored in the 'public span' of the memory
	// region. As it stands, the free data is split across (when allocated) the 
	// allocation_header and allocation data.
	//
	// We prepared for the necessary alignment when we calculated the available
	// size above; therefore, just find the maximum extent of the pool node list,
	// align to the next 'default_align' boundary, and use that as the
	// 'free_data' pointer.
	void* public_span_start =
		align_pointer((char*)node + node->size * sizeof(pool_node), default_align);

	// Adjust by the difference between 'default_align' and
	// sizeof(allocation_header).
	//
	// The allocation header is stored right before the allocation, which means
	// there could be necessary padding on platforms where the default alignment is
	// different from the size of the allocation header.
	node->free_data = 
		(free_pool_node*)((char*)public_span_start + (default_align - sizeof(allocation_header)));

	// Initialize the free_data member.
	node->free_data->memory_region = requested_region_header;
	node->free_data->current = node;
	node->next = nullptr;
	node->previous = nullptr;

	// Finish creating the memory region header.
	//
	// The first free node spans the available region, thus its size is the
	// maximum number of objects that can be allocated. So just use that value for
	// the memory_region_header::size.
	requested_region_header->size = node->size;
	requested_region_header->free_nodes = node->size;
	requested_region_header->free_pool_node_list = node->free_data;

	if (memory_region_list_head == nullptr)
	{
		// No regions have been allocated, so initialize the list.
		memory_region_list_head = requested_region_header;
		memory_region_list_tail = requested_region_header;
	}
	else
	{
		// There are regions in use! So just update the tail.
		memory_region_list_tail =
			intrusive_list::insert_after(requested_region_header, memory_region_list_tail);
	}

	// We're done. Return the new memory region.
	return requested_region_header;
}

void holo::pool_allocator::make_free_node(void* pointer)
{
	free_pool_node* free_data = (free_pool_node*)pointer;

	// Find the free node that is just before the provided node.
	//
	// Although this is a linear search, it should rarely be too slow because of
	// the way allocatinos are handled. Spans of more than one free object are
	// coalesced into 'large' free nodes, reducing the cost of iteration over
	// empty spans.
	//
	// This coalescing operation is performed first; thus, if there is a free node
	// immediately to the left (i.e., node->previous is free), then the search
	// will never be performed, eliminating a possible worst-case scenario where 
	// every other node is allocated is no longer a problem.
	//
	// The worst case scenario would be an alternating pattern of allocations
	// prior to the free node, which itself is adjacent to an allocated node on
	// the left side.
	//
	// When instrusive AVL trees (and thus binary trees) are implemented, then
	// the free list can become a tree, resulting in speedy insertions regardless
	// of the allocation pattern.
	auto prior_free_node_iter =
		intrusive_list::make_iterator(free_data->memory_region->free_pool_node_list);
	auto end_free_node_iter = intrusive_list::end<free_pool_node>();

	free_pool_node* prior_free_node = nullptr;
	while (prior_free_node_iter != end_free_node_iter)
	{
		// 'current' means 'current relative to this loop'; not to be confused with
		// the node we are generating 'free_data' for.
		free_pool_node* current_free_node = *prior_free_node_iter;

		if (prior_free_node > free_data)
		{
			// We've gone too far. The 'prior_free_node' will contain the closest node
			// before 'node' (the parameter) since it hasn't been updated.
			break;
		}

		prior_free_node = current_free_node;
		prior_free_node_iter++;
	}

	if (prior_free_node == nullptr)
	{
		// If prior_free_node is null, that means there are no allocations currently
		// in the free list, so just insert node->free_data.
		free_data->next = nullptr;
		free_data->previous = nullptr;

		free_data->memory_region->free_pool_node_list = free_data;
	}
	else
	{
		// Otherwise, just insert the free_data into the list.
		intrusive_list::insert_after(free_data, prior_free_node);
	}

	// Mark the pool node as freed.
	free_data->current->free_data = free_data;
}
