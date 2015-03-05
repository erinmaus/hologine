// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_MEMORY_POOL_ALLOCATOR_HPP_
#define HOLOGINE_CORE_MEMORY_POOL_ALLOCATOR_HPP_

#include <cstddef>
#include "core/memory/allocator.hpp"
#include "core/memory/memory_arena_pool.hpp"

namespace holo
{
	// Allocates blocks of memory with uniform sizes.
	class pool_allocator final : public allocator
	{
		public:
			// Constructs a pool allocator with the provided
			// holo::memory_arena_pool with objects no larger than
			// 'object_size' bytes. This value must be a multiple of the default
			// alignment.
			//
			// There is a lower limit to 'object_size', depending on the platform.
			// 'object_size' is clamped to this limit, thus the object size for a
			// pool may be larger than requested.
			pool_allocator(holo::memory_arena_pool* arena_pool, std::size_t object_size);
			
			// Releases all memory associated with this pool allocator.
			//
			// The holo::memory_arena_pool instance must have a lifetime
			// greater than the holo::pool_allocator, since upon destruction, all
			// memory regions will be returned.
			~pool_allocator();
			
			// Allocates a single block from the pool.
			//
			// Returns a pointer to the base of the block on success, NULL on failure.
			//
			// Failure can occur if there is no more memory that can be obtained from
			// the system, in which case a platform-specific error will be propogated
			// from the underlying holo::memory_region.
			//
			// This method will fail if 'size' is greater than the requested block size
			// (see holo::pool_allocator::pool_allocator(holo::memory_arena_pool*, size_t)
			// for more information). In such a case,
			// holo::exception::invalid_argument will be pushed.
			//
			// Per-allocation alignment is ignored. All objects will be aligned to
			// the default alignment boundary.
			void* allocate(std::size_t size, std::size_t alignment = default_alignment);
			
			// Deallocates a block from a previous call to
			// holo::pool_allocator::allocate(size_t, size_t).
			void deallocate(void* pointer);

			// Gets the size of an object.
			//
			// This value may be larger than the one provided in the constructor
			// because of implementation details.
			std::size_t get_object_size() const;

			// Gets the maxmimum number of objects that can be stored in one arena.
			std::size_t get_object_count() const;

		private:
			typedef holo::memory_arena_pool::arena_record arena_record;
			typedef holo::memory_arena_pool::allocator_free_node free_node;

			// Gets the first memory arena available to back a requseted allocation,
			// starting the search at the provided arena.
			//
			// This method's speed depends on how many full memory arenas there
			// are and is thus O(n). However, if the provided region is known to
			// have a free pool node, it will be O(1).
			//
			// When the node is retrieved, it should be marked as allocated (that is,
			// the arena's capacity should be decreased).
			//
			// If a free node could not be found, this returns NULL. Otherwise,
			// returns the first free pool node of the first memory region available.
			free_node* get_first_free_node(arena_record* record);

			// Requests a new arena from the free list.
			//
			// On success, the new arena will be the new tail of the memory region
			// list.
			//
			// Returns a pointer to the arena record on success, NULL on failure.
			// Failure can occur if the system memory is exhausted.
			arena_record* request_empty_arena();

			// Pointer to the first arena record.
			arena_record* arena_list_head;

			// Pointer to the last arena record.
			arena_record* arena_list_tail;
			
			// The arena pool.
			//
			// From the pool we retrieve arenas, and once an arena reaches the end of
			// its life, it returns to the pool.
			holo::memory_arena_pool* memory_arena_pool;
			
			// The maximum size of an object stored in this pool.
			std::size_t object_size;

			// The maximum number of objects stored in the pool.
			std::size_t object_count;
	};
}

#endif
