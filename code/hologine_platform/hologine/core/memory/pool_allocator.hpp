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
#include "core/test.hpp"

HOLOGINE_DECLARE_INTERNAL_TEST(pool_allocator);

namespace holo
{
	class heap_allocator;
	class memory_region_free_list;

	// Allocates blocks of memory with uniform sizes.
	class pool_allocator final : public allocator
	{
		HOLOGINE_ENABLE_INTERNAL_TEST(pool_allocator);

		public:
			// Constructs a pool allocator with the provided
			// holo::memory_region_free_list with objects no larger than
			// 'object_size' bytes. This value must be a multiple of the default
			// alignment.
			//
			// There is a lower limit to 'object_size', depending on the platform.
			// 'object_size' is clamped to this limit, thus the object size for a
			// pool may be larger than requested.
			pool_allocator(memory_region_free_list* free_list, std::size_t object_size);
			
			// Releases all memory associated with this pool allocator.
			//
			// The holo::memory_region_free_list instance must have a lifetime
			// greater than the holo::pool_allocator, since upon destruction, all
			// memory regions will be returned to the free list.
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
			// (see holo::pool_allocator::pool_allocator(holo::memory_region_free_list*, size_t)
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

		private:
			struct free_pool_node;
			struct memory_region_header;

			// Per memory region node representing an allocation or deallocated
			// span.
			//
			// A list of pool_node objects are stored at the beginning of a memory
			// region, after the header.
			struct pool_node
			{
				// The size of the node.
				//
				// This is equal to 1 for an allocated node, but for free nodes, this
				// value is the size of the free memory region as a multiple of
				// 'object_size.' Thus, if 'size' is 16 and 'object_size' is 32, the
				// region would be 512 bytes large.
				//
				// Deallocated nodes are initially coalesced for a multiple most notably
				// to ensure fast initialization of new memory regions. Rather than
				// initialize a higher number of nodes, we just need to initialize one
				// and be done.
				//
				// When size is the size of memory regions returned by the free list,
				// and if this node is the first node in a memory region, then the
				// memory region this node belongs to can be returned to the free list.
				std::size_t size;

				// Pointer to 'free_pool_node' data.
				//
				// If NULL, this node is allocated.
				free_pool_node* free_data;

				// Pointer to the next node in this memory region.
				//
				// This could be a free node or an allocated node.
				pool_node* next;
				
				// Pointer to the previous node in this memory region.
				//
				// This could be a free node or an allocated node.
				pool_node* previous;
			};
			
			// Extra data used for deallocated nodes.
			//
			// This node is stored in the public span (the portion of memory returned
			// upon a successful allocation).
			//
			// This does mean that a stale pointer can trash the free node list...
			// Keep this in mind while debugging.
			struct free_pool_node
			{
				// The memory region this free node belongs to.
				//
				// This corresponds to allocation_header::memory_region.
				memory_region_header* memory_region;

				// Pointer to the current pool node; that is, the deallocated pool node
				// that stores more information about this instance's state.
				//
				// This corresponds to allocation_header::node.
				pool_node* current;

				// Pointer to the next free node in this memory region.
				free_pool_node* next;
				
				// Pointer to the previous free node in this memory region.
				free_pool_node* previous;
			};
			
			// Memory region header state.
			struct memory_region_header
			{
				// Next memory region in use by this pool allocator,
				memory_region_header* next;
				
				// Previous memory region in use by this pool allocator.
				memory_region_header* previous;

				// Maximum number of objects that can be stored in the memory region.
				//
				// This value could potentially change between regions returned by
				// the free list, depending on platform quirks, so just play it safe and
				// store the limit in each region.
				std::size_t size;

				// Number of freed objects.
				//
				// If this number equals 'size', then the region header is empty and it
				// can potentially be returned to the free list.
				std::size_t free_nodes;

				// First block of unallocated memory in this region.
				free_pool_node* free_pool_node_list;
			};

			// Header for allocations.
			//
			// This is stored just before the pointer returned by
			// holo::pool_allocator::allocate(size_t, size_t).
			//
			// By design, an allocation_header is equivalent to the first members of
			// a free_pool_node. The reason is that so when a free node is initialized,
			// the allocation record will also be initialized. This is because both
			// the allocation_header and corresponding free_pool_node start at the
			// same location in memory.
			//
			// The size of this structure must be less than or equal to the default
			// alignment, because this structure is intended to fit in between the
			// padding created by alignment requirements.
			//
			// Luckily for us, this should be the case on all but the most esoteric
			// of platforms:
			//
			// On supported 32-bit platforms, such as x86, pointers are 4 bytes and 
			// alignment is normally 8 bytes. This structure has two pointers and thus
			// is 8 bytes.
			//
			// Similarly, on 64-bit platforms (x86_64), pointers are now 8 bytes, but
			// default alignment is now 16 bytes!
			//
			// The reason for this requirement is so objects are aligned along the
			// default alignment always.
			struct allocation_header
			{
				// Pointer to the memory region this allocation belongs to.
				memory_region_header* memory_region;

				// Pointer to the pool node that has more information about this
				// allocation.
				pool_node* node;
			};

			// Gets the first memory region available to back a requseted allocation,
			// starting the search at the provided memory region.
			//
			// This method's speed depends on how many full memory regions there
			// are and is thus O(n). However, if the provided region is known to
			// have a free pool node, it will be O(1).
			//
			// If a free node could not be found, this returns NULL. Otherwise,
			// returns the first free pool node of the first memory region available.
			pool_node* get_first_free_node(memory_region_header* header);

			// Requests a new memory region from the free list.
			//
			// On success, the new memory region will be the new tail of the memory
			// region list.
			//
			// Returns a pointer to the memory region header on success, NULL on
			// failure. Failure can occur if the system memory is exhausted.
			memory_region_header* request_empty_memory_region();

			// Generates 'free_data' for a node when a 'coalesce_before' operation
			// cannot be performed.
			//
			// 'pointer' should be the location where the 'free_data' should be stored
			// (i.e., the location of the allocation_header).
			void make_free_node(void* pointer);
			
			// Pointer to the first memory region.
			//
			// Memory regions are created lazily; the memory region will be created
			// only if an allocation is requested. Afterwards, at least one memory
			// region will be kept around until the pool allocator is destructed, at
			// which point all memory regions will be returned to the free list.
			memory_region_header* memory_region_list_head;

			// Pointer to the last memory region.
			memory_region_header* memory_region_list_tail;
			
			// The free list.
			//
			// This is the object that manages free memory regions. When a memory
			// region is empty, it should be returned to the free list; similarly,
			// when a memory region is needed, it should be requested from the free
			// list.
			holo::memory_region_free_list* memory_region_free_list;
			
			// The maximum size of an object stored in this pool.
			std::size_t object_size;
	};
}

#endif
