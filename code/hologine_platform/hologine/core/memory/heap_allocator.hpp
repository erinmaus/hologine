// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_MEMORY_HEAP_ALLOCATOR_HPP_
#define HOLOGINE_CORE_MEMORY_HEAP_ALLOCATOR_HPP_

#include <cstddef>
#include "core/memory/allocator.hpp"
#include "core/memory/buffer.hpp"
#include "core/memory/memory_arena_pool.hpp"
#include "core/memory/pool_allocator.hpp"

namespace holo
{
	class pool_allocator;

	// Provides an interface to a general allocation strategy.
	class heap_allocator final : public allocator
	{
		public:
			// Constructs a heap allocator with the provided parameters.
			//
			// 'arena_size' determines the minimum size of memory arenas
			// allocated by the allocator and must be a power of two.
			//
			// 'arena_count' determines how many memory regions are permitted in
			// total.
			//
			// 'pool_start' determines the minimum size of a pool while 'pool_end'
			// determines the maximum size. Small allocations (that is, allocations
			// less than 'pool_end') may be stored in a pool, resulting in a faster
			// allocation pattern. The behavior is implementation-specific, however.
			//
			// Therefore, the only guarantee is that a number of pools will be created
			// to fit smaller allocations, and that the largest pool will be
			// 'pool_end' bytes large, while the smallest will be 'pool_start'.
			//
			// Note: size parameters are in bytes.
			explicit heap_allocator(
				std::size_t arena_size = 0x40000u,
				std::size_t arena_count = 0x100u,
				std::size_t pool_start = 0x20u,
				std::size_t pool_end = 0x10000u);

			// Frees all memory allocated.
			//
			// Any memory allocated by this instance is no longer valid.
			~heap_allocator();

			// Allocates a block of memory from the heap allocator.
			//
			// If the allocation could not be made, then this method returns NULL and
			// an appropriate exception is pushed.
			void* allocate(std::size_t size, std::size_t alignment = default_alignment);

			// Deallocates a block of memory previously obtained from this allocator.
			void deallocate(void* pointer);

		private:
			// The maximum number of pools allocated by the heap allocator.
			//
			// Keep in mind each pool is a power of two. The last pool, in this
			// pattern, would make 4 gigabyte allocations for one object! 32 max pools
			// is more than plenty.
			static const std::size_t maximum_pool_count = 32;

			// The memory arena pool.
			holo::memory_arena_pool memory_arena_pool;

			// Static buffer used to generate an array of pool allocators.
			holo::buffer<sizeof(holo::pool_allocator) * maximum_pool_count> pool_allocators_buffer;

			// Pointer to an array of pool allocators.
			//
			// This is the value of pool_allocator_buffer::get().
			pool_allocator* pool_allocators;

			// Minimum size of a pool, represented as the log2 (e.g., 32 would be 5).
			std::size_t minimum_pool_size;

			// Maximum size of a pool, represented as the log2 (e.g., 8192 would be
			// 13).
			std::size_t maximum_pool_size;
	};
}

#endif
