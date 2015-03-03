// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_MEMORY_FIXED_ALLOCATOR_HPP_
#define HOLOGINE_CORE_MEMORY_FIXED_ALLOCATOR_HPP_

#include <cstddef>
#include "core/memory/allocator.hpp"
#include "core/memory/memory_region.hpp"

namespace holo
{
	// Provides an interface for a fixed-size arena of fixed-size objects.
	//
	// This is an optimization of holo::pool_allocator when all allocations are
	// exactly the same size. Maybe holo::pool_allocator should be renamed to
	// holo::bin_allocator...
	class fixed_allocator final : public allocator
	{
		public:
			// Constructs the fixed allocator, restricted to 'size' bytes of storage.
			//
			// 'size' must be greater than or equal to sizeof(void*).
			//
			// All objects will be aligned to 'alignment' boundary. This may or may
			// not introduce overhead padding between allocations. To ensure objects
			// are tightly packed, ensure 'object_size' bytes is a power of two, and
			// that 'alignment' is smaller than or equal to 'object_size', or that
			// 'object_size' is a multiple of 'alignment'. As long as the object size
			// is less than a page boundary, the allocations will be packed tightly in
			// either case.
			//
			// If 'size' is not a multiple of object size (including potential
			// alignment requirements), then there will be wasted space.
			fixed_allocator(
				std::size_t size,
				std::size_t object_size,
				std::size_t alignment = default_alignment);

			// Frees all allocated memory.
			~fixed_allocator();

			// Allocates an object.
			//
			// 'size' must be less than or equal to the 'object_size' provided in the
			// constructor. Alignment is ignored.
			void* allocate(std::size_t size, std::size_t alignment = default_alignment);

			// Deallocates a previously allocated object.
			void deallocate(void* pointer);

		private:
			// Memory region that provides the backing store to the fixed allocator.
			holo::memory_region memory_region;

			// Pointer to the base of the memory region.
			//
			// This may not be the value returned by holo::memory_region::claim() if
			// alignment is greater than page size.
			void* memory;

			// Free node.
			struct free_node
			{
				// Next node in the free list.
				free_node* next;
			};

			// List of free nodes.
			//
			// This list not ordered. Instead, the first node will be the most
			// recently deallocated node...
			free_node* free_nodes;

			// Node size, in bytes.
			//
			// This value includes any necessary padding.
			std::size_t node_size;

			// Object size, in bytes.
			std::size_t object_size;
	};
}

#endif
