// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include "core/math/util.hpp"
#include "core/memory/fixed_allocator.hpp"
#include "core/memory/memory_region.hpp"

holo::fixed_allocator::fixed_allocator(
	std::size_t size,
	std::size_t object_size,
	std::size_t alignment) :
		memory_region(size),
		memory(nullptr),
		free_nodes(nullptr),
		object_size(object_size)
{
	holo_assert(object_size >= sizeof(free_node));

	void* base_memory = memory_region.claim();
	if (base_memory != nullptr)
	{
		// There is one problem: if 'alignment' is greater than or equal to the size
		// of the memory region, the node span calculation will be terribly wrong.
		memory = align_pointer(base_memory, alignment);

		// Determine the size of a node, including padding.
		if (math::modulo_power_of_two(object_size, alignment) == 0)
		{
			// The node can be tightly packed.
			node_size = object_size;
		}
		else
		{
			// Calculate the padding between nodes.
			//
			// This is simply 'object_size' rounded up to the next multiple of
			// alignment.
			node_size = math::round_up(object_size, alignment);
		}

		// Determine how much space is available for nodes.
		std::size_t max_size = memory_region.get_current_size();
		std::size_t node_span_size = max_size - get_pointer_distance(memory, base_memory);
		std::size_t node_count = node_span_size / node_count;

		// Reserve 'node_count' nodes, starting backwards.
		//
		// Remember, the free node list is in order of most recent to least recent.
		// A naive forward iteration would put the nodes at the end of the region
		// which could potentially be counterproductive if nodes are used linearly,
		// from low to high.
		char* current_node = (char*)memory + (node_count - 1) * node_size;
		while (current_node > base_memory)
		{
			// holo::fixed_allocator::deallocate will happily convert any pointer to
			// a free node. Make use of that behavior.
			deallocate(current_node);

			current_node -= node_size;
		}
	}
}

void* holo::fixed_allocator::allocate(std::size_t size, std::size_t)
{
	if (size > object_size)
	{
		push_exception(exception::invalid_argument);

		return nullptr;
	}

	free_node* current_free_node = free_nodes;

	if (current_free_node != nullptr)
	{
		free_nodes = current_free_node->next;
	}
	else
	{
		push_exception(exception::out_of_memory);
	}

	return current_free_node;
}

void holo::fixed_allocator::deallocate(void* pointer)
{
	// We don't verify if pointer belongs to the fixed allocator... Maybe we
	// should in the future.
	free_node* new_free_node = (free_node*)pointer;

	new_free_node->next = free_nodes;
	free_nodes = new_free_node;
}
