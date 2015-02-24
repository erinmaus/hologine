// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_MEMORY_MEMORY_REGION_FREE_LIST_HPP_
#define HOLOGINE_CORE_MEMORY_MEMORY_REGION_FREE_LIST_HPP_

#include <cstddef>
#include "core/memory/memory_region.hpp"

namespace holo
{
	// Represents a means to manage holo::memory_region objects effectively
	// between allocators.
	//
	// Although allocators have different behaviors, they all need memory comitted
	// from the host system. When an allocator no longer needs memory (i.e., the
	// requested region has been emptied of all active allocations), the
	// holo::memory_region objects can be shared between allocators, thus wasting
	// less memory.
	//
	// This is most effectively demonstrated in the holo::heap_allocator
	// implementation, which makes use of a variety of holo::pool_allocators that
	// share a holo::memory_region_free_list; refer to holo::heap_allocator for
	// more information.
	class memory_region_free_list final
	{
		memory_region_free_list(const memory_region_free_list&) = delete;
		memory_region_free_list& operator =(const memory_region_free_list&) = delete;
		
		public:
			// Constructs a free list for holo::memory_region objects of the provided
			// hint, in bytes.
			//
			// A holo::memory_region is not allocated until
			// holo::memory_region_free_list::pop() is called. Similarly, no errors
			// will be pushed on to the exception stack by the constructor.
			//
			// A portion of the memory region returned is used by the memory region
			// free list. To retrieve the available portion of a memory region, query
			// holo::memory_region_free_list::get_memory_region_size().
			explicit memory_region_free_list(std::size_t hint);
			
			// Destructs all memory regions currently in the free list.
			//
			// The behavior of the destructor is identical to
			// holo::memory_region_free_list::reset().
			~memory_region_free_list();

			// Reserves a list of regions for use.
			//
			// Returns the number of regions reserved. If this number is less than
			// the requested region count, then this method could not successfully
			// reserve enough regions. In such a case, an exception will be at the
			// top of the stack indicating the reason.
			std::size_t reserve(std::size_t count);
			
			// Pops a free memory region from the list and returns a pointer to the
			// beginning of it.
			//
			// This method will create a holo::memory_region if no free regions are
			// currently contained within the list.
			//
			// The memory region will have all memory claimed. Therefore, the
			// returned value will be at least 'size' bytes large, determined by the
			// page boundary.
			//
			// Returns a pointer into a holo::memory_region on success; otherwise
			// returns NULL on failure. Failure can occur if there is no more memory
			// available to the system. To query for the exact reason, call
			// holo::get_last_exception().
			void* pop();
			
			// Pushes a memory from previously created by the list, marking it as
			// free.
			//
			// The argument is expected to be a pointer from a previous call to
			// holo::memory_region_free_list::pop().
			//
			// This method performs no error-checking as to the source of the region.
			void push(void* region_base);
			
			// Releases all freed memory regions back to the system.
			//
			// Any regions still in use are not freed; only regions stored in this
			// list at the time this method is called.
			void reset();
			
			// Gets the size, in bytes, of a memory region.
			//
			// This value is adjusted from the requested size to fit fit best within
			// the platform's virtual memory requirements, and then reduced for
			// bookkeeping by the free list.
			std::size_t get_memory_region_size() const;
		
		private:
			// Allocates a new memory region, adding it to the free list.
			//
			// Returns true on success, false otherwise. On failure, the reason is
			// propagated from the underlying method call.
			bool allocate_new_region();

			// Represents the header of a memory region.
			struct memory_region_header
			{
				// The actual holo::memory_region object is stored at the beginning of
				// the memory region upon a successful allocation.
				holo::memory_region memory_region;
				
				// A pointer to the next free memory region.
				//
				// This value is only valid when the memory region is stored in the free
				// list.
				memory_region_header* next;
			};
			
			// The head of the free list.
			//
			// When the free list is empty, this value should be NULL.
			memory_region_header* free_list_head;
			
			// The size of memory regions stored in this list, in bytes.
			//
			// This value will be the sum of a memory region header and the requested
			// free list size.
			//
			// In the current implementation, the simple sum actually wastes an extra
			// four or eight bytes, in 32-bit and 64-bit builds respectively, when the
			// memory region is active. This is because the memory_region_header::next
			// member is discarded.
			std::size_t size;
			
			// The size of the memory region available to the user, in bytes.
			std::size_t user_size;
	};
}

#endif
