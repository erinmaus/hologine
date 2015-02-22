// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_MEMORY_MEMORY_REGION_INTERFACE_HPP_
#define HOLOGINE_CORE_MEMORY_MEMORY_REGION_INTERFACE_HPP_

#include <cstddef>

namespace holo
{
	// Represents the platform-specific implementation of a memory region and
	// exposes the necessary methods.
	class memory_region_interface
	{
		public:
			// Reserves 'max_pages' of virtual memory.
			//
			// These pages should not yet be committed.
			//
			// Returns a pointer to the beginning of the virtual memory region on
			// success, NULL on failure.
			virtual void* reserve_pages(std::size_t max_pages) = 0;
			
			// Releases the previously reserved pages in the provided range.
			virtual void release_pages(void* base, std::size_t index, std::size_t count) = 0;
			
			// Commits a range of pages.
			virtual bool commit_pages(void* base, std::size_t index, std::size_t count) = 0;
			
			// Decommits a range of pages.
			virtual void decommit_pages(void* base, std::size_t index, std::size_t count) = 0;
			
			// Gets the size of a page on the platform, in bytes.
			virtual std::size_t get_page_size() const = 0;
	};
}

#endif
