// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_MEMORY_MEMORY_REGION_HPP_
#define HOLOGINE_CORE_MEMORY_MEMORY_REGION_HPP_

#include <cstddef>
#include "core/memory/allocator.hpp"
#include "core/memory/memory_region_base.hpp"

namespace holo
{
	// Reserves and commits virtual memory directly from the underlying platform.
	//
	// Not to be confused with holo::allocator. These two objects refer to
	// different ideas of memory allocation. A holo::memory_region reserves
	// a range of virtual memory from the underlying platform and commits
	// portions of it as necessary; see 
	// holo::memory_region::grow(std::size_t) for more information.
	//
	// The memory region is a region of virtual memory Up to the requested size
	// of bytes will be reserved, rounded to the nearest page. These pages are
	// not comitted or otherwise usuable until requested. Pages are committed in
	// a linear manner, starting from the base of the virtual memory region and
	// growing towards the end of it.
	//
	// Individual pages cannot be deallocated. In order to decommit pages, the
	// region must be reset. This effectively frees all committed pages.
	class memory_region final : private memory_region_base
	{
		memory_region(const holo::memory_region&) = delete;
		holo::memory_region& operator =(const holo::memory_region&) = delete;
		
		public:
			// Move constructor.
			//
			// After a move, 'other' will be an empty memory region.
			memory_region(memory_region&& other);
			
			// Move assignment operator.
			//
			// After a move, 'other' will be an empty memory region.
			memory_region& operator =(memory_region&& other);
			
			// Constructs an empty memory region.
			//
			// The memory region will reserve no virtual memory and thus cannot have
			// any memory committed.
			//
			// This is equivalent of calling holo::memory_region::memory_region(0).
			memory_region();
			
			// Constructs a memory region.
			//
			// The region will be large enough to store max_size bytes. Keep in mind,
			// due to differences in the size of a page, the actual region reserved
			// may be larger or smaller. To query the final size of the memory
			// region, see holo::memory_region::get_reserved_size().
			memory_region(std::size_t max_size);
			
			// Decommits and releases the virtual memory region represented by this
			// object.
			//
			// All pointers into the memory region are invalid, and will likely cause
			// an access violation. To prevent this, manage memory properly!
			~memory_region();
			
			// Claims the memory region, commiting the entire region for use.
			//
			// This method commits the entire region of memory at one time, rather
			// than increasing it as necessary (see holo::memory_region::grow(size_t).
			//
			// Calling holo::memory_region::claim() after a call to
			// holo::memory_region::grow(size_t) will finish commiting the region. If
			// the entire region has been committed, this method will fail.
			//
			// Returns a pointer to the beginning of the memory region on success, or
			// NULL on failure. Failure can occur if the underlying region was unable
			// to be reserved; or if the memory could not be committed; or the entire
			// region has already been committed; or for some platform-specific
			// reason.
			//
			// To query why this method may have failed and thus returned NULL,
			// use holo::get_last_exception().
			void* claim();
			
			// Commits more memory to ensure space for 'size' more bytes.
			//
			// This method will commit pages of memory, if necessary, unless
			// the maximum size has been reached; in such an event, this method will
			// return NULL. Otherwise, returns a pointer to the base of the virtual
			// memory region.
			//
			// Note: if memory has already successfully been committed and 'size' is
			// 0, then the pointer to the base of the virtual memory region will be
			// returned. If no memory has been committed, either because this method
			// has yet to be called or growing failed, then this will return NULL.
			//
			// Similarly, only fully resetting the memory region will invalidate the
			// return pointer. Thus, if 'size' is 0 after calling reset(false), this
			// will still return the base of the virtual memory region, although
			// since no memory is allocated, dereferencing the pointer will cause
			// an access violation.
			//
			// If this method fails, an appropriate error message will be pushed.
			void* grow(std::size_t size);
			
			// Resets the memory region.
			//
			// All memory previously allocated is considered free. As well, the
			// virtual memory region reserved will be released if 'release' is true;
			// otherwise, the region will remain reserved (but uncomitted).
			void reset(bool release);
			
			// Gets the maximum size of the memory region, in bytes.
			//
			// This value may be different than the value passed in the constructor
			// due to the page-size differences between platforms. For example, if
			// 4000 bytes were requested upon the creation of the memory region, and
			// the size of a page on the host system is 4096 bytes, then the maximum
			// size of the region will be rounded to 4096.
			//
			// In short, this method returns the requested size of the region rounded
			// to the nearest page, in bytes.
			std::size_t get_reserved_size() const;
			
			// Gets the current size of the memory region, in bytes.
			//
			// This value will be a multiple of the page size. Refer to
			// holo::memory_region::get_reserved_size() for more information.
			std::size_t get_current_size() const;
		
		private:
			// The largest this region can grow, in bytes.
			//
			// The actual value may be larger than requested; refer to
			// holo::memory_region::get_reserved_size() for more information.
			std::size_t max_size;
			
			// The current requested size of the region, in bytes.
			//
			// The number of pages actually reserved can be retrieved by dividing the
			// return value of holo::memory_region::get_current_size() by the
			// return value of holo::memory_region::get_page_size().
			std::size_t current_size;
			
			// Pointer to the beginning of the virtual memory region; will be NULL
			// if the memory has yet to be reserved.
			void* memory;
	};
}

#endif
