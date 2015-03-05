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
#include "core/memory/memory_arena_pool.hpp"

holo::pool_allocator::pool_allocator
	(holo::memory_arena_pool* memory_arena_pool, std::size_t object_size) :
		arena_list_head(nullptr),
		arena_list_tail(nullptr),
		memory_arena_pool(memory_arena_pool),
		// Increase the object size to fit a free_node. This is necessary because
		// when an object is returned to the pool, its memory is re-used to store a
		// a portion of a free_pool_node, and thus must be at least as large as the
		// portion of the free_node stored.
		object_size(std::max(object_size, sizeof(free_node))),
		object_count(memory_arena_pool->get_arena_size() / object_size)
{
	holo_assert(memory_arena_pool != nullptr);
}

holo::pool_allocator::~pool_allocator()
{
	arena_record* current = arena_list_head;
	
	// Return the memory arenas to the free list.
	while (current != nullptr)
	{
		// Any references to the header are forfeit once it is returned.
		arena_record* next = current->next;

		memory_arena_pool->give_arena(current);

		current = next;
	}
}

void* holo::pool_allocator::allocate(std::size_t size, std::size_t)
{
	// Sanity check.
	if (size > object_size)
	{
		// The allocation cannot succeed. Allocated blocks are assumed to be no more
		// than 'object_size' bytes large.
		push_exception(holo::exception::invalid_argument);
		
		return nullptr;
	}

	free_node* node = get_first_free_node(arena_list_head);
	if (node == nullptr)
	{
		// There are no more empty arenas. Request one from the pool.
		arena_record* new_arena = request_empty_arena();

		if (new_arena == nullptr)
		{
			// Well, there's no more memory.
			return nullptr;
		}

		node = get_first_free_node(new_arena);
	}
	holo_assert(node != nullptr);

	// holo::pool_allocator::get_first_free_node(arena_record*) should somehow
	// return the record it chose as well...
	arena_record* arena = memory_arena_pool->get_arena(node);
	holo_assert(arena);

	free_node* next;
	if (node->size > 1)
	{
		// This is a lazy node from when the arena was first requested. Create a
		// 'next' free node by splitting the span of unallocated memory.
		next = (free_node*)((char*)node + object_size);
		next->size = node->size - 1;
		next->next = node;
	}
	else
	{
		next = node->next;
	}
	next->previous = node->previous;

	// Update the previous node and the arena free list.
	free_node* previous = node->previous;
	if (arena->free_node_count == 0)
	{
		// The arena is exhausted, thus the free list should be empty.
		arena->free_node_list = nullptr;
	}
	else
	{
		previous->next = next;
		arena->free_node_list = next;
	}

	// Memory allocation is good to go! Return.
	return node;
}

void holo::pool_allocator::deallocate(void* pointer)
{
	arena_record* arena = memory_arena_pool->get_arena(pointer);

	// Although not normally a sane option, we allow 'pointer' to be different
	// from the value returned by
	// holo::pool_allocator::allocate(std::size_t, std::size_t) for one reason:
	// the generic heap allocator may round up an allocation based on alignment
	// requirements.
	//
	// If we required the normal behavior (pointer must equal return value of the
	// allocation method), then the generic heap allocator would have to use extra
	// data to keep track of allocations... By finding the pointer, we eliminate
	// the bookkeeping!
	pointer = (void*)(((unsigned_pointer)pointer - (unsigned_pointer)arena->base) % object_size);

	holo_assert(arena != nullptr);
	holo_assert(arena->allocator == this);

	++arena->free_node_count;

	// Return the arena immediately if possible.
	if (arena->free_node_count == object_count)
	{
		// Remove the arena from the list first.
		intrusive_list::remove(arena);

		// Update the head and tail pointers, if necessary.
		if (arena_list_head == arena)
		{
			arena_list_head = arena->next;
		}

		if (arena_list_tail == arena)
		{
			arena_list_tail = arena->next;
		}

		memory_arena_pool->give_arena(arena);
	}
	else
	{
		// Otherwise, update the arena's free node list.
		free_node* node = (free_node*)pointer;

		// Size would have been overwritten by any data stored in the object (so we
		// must assume).
		node->size = 1;

		if (arena->free_node_list != nullptr)
		{
			node->next = arena->free_node_list;
			node->previous = arena->free_node_list->previous;
		}
		else
		{
			// This is the first deallocated node in the arena now. Since the free
			// node list is a circular buffer, 'node' should point to itself.
			node->next = node;
			node->previous = node;
		}
	}
}

std::size_t holo::pool_allocator::get_object_size() const
{
	return object_size;
}

std::size_t holo::pool_allocator::get_object_count() const
{
	return object_count;
}

holo::pool_allocator::free_node* holo::pool_allocator::get_first_free_node(arena_record* arena)
{
	arena_record* current_arena = arena;

	while (current_arena != nullptr)
	{
		if (current_arena->free_node_list != nullptr)
		{
			// Update the arena's free node count.
			--current_arena->free_node_count;

			return current_arena->free_node_list;
		}

		current_arena = current_arena->next;
	}

	return nullptr;
}

holo::pool_allocator::arena_record* holo::pool_allocator::request_empty_arena()
{
	arena_record* arena = memory_arena_pool->take_arena();

	if (arena != nullptr)
	{
		arena->allocator = this;
		arena->free_node_count = object_count;
		arena->free_node_list = (free_node*)arena->base;

		arena->free_node_list->size = object_count;
		arena->free_node_list->next = arena->free_node_list;
		arena->free_node_list->previous = arena->free_node_list;

		if (arena_list_tail == nullptr)
		{
			// No arenas have been reserved by the pool.
			arena_list_head = arena;
			arena_list_tail = arena;
		}
		else
		{
			intrusive_list::insert_after(arena, arena_list_tail);	
		}
	}

	return arena;
}
