// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include <utility>
#include "core/exception.hpp"
#include "core/math/util.hpp"
#include "core/memory/memory_region.hpp"

holo::memory_region::memory_region(holo::memory_region&& other) :
	max_size(other.max_size),
	current_size(other.current_size),
	memory(other.memory)
{
	other.max_size = 0;
	other.current_size = 0;
	other.memory = nullptr;
}

holo::memory_region& holo::memory_region::operator =(holo::memory_region&& other)
{
	max_size = other.max_size;
	current_size = other.current_size;
	memory = other.memory;
	
	other.max_size = 0;
	other.current_size = 0;
	other.memory = nullptr;

	return *this;
}

holo::memory_region::memory_region() :
	max_size(0), current_size(0), memory(nullptr)
{
	// Nothing.
}

holo::memory_region::memory_region(std::size_t max_size) :
	max_size(max_size), current_size(0), memory(nullptr)
{
	// Nothing.
}

holo::memory_region::~memory_region()
{
	// Easy-peasy.
	reset(true);
}

void* holo::memory_region::claim()
{
	// Calculate the remaining portion that needs committed.
	std::size_t remaining_size = max_size - current_size;
	
	if (remaining_size == 0)
	{
		// The region has been fully committed already. Return NULL.
		push_exception(exception::invalid_operation);
		
		return nullptr;
	}
	
	// Simply let holo::memory_region::grow(size_t) do the rest.
	return grow(remaining_size);
}

void* holo::memory_region::grow(std::size_t size)
{
	// The memory region is only initialized when necessary.
	// Regardless of if the region needs to grow (i.e., size == 0), allocate
	// resources for the region from the underlying platform if the instance has
	// yet to do so.
	if (memory == nullptr)
	{
		// A region that has no size (that is, 'max_size' is 0) should simply return
		// NULL.
		if (max_size == 0)
		{
			return nullptr;
		}
		
		memory = reserve_pages(math::multiple_of(max_size, get_page_size()));
		
		// Failed to reserve pages; return NULL and hope the caller can figure
		// out the rest.
		if (memory == nullptr)
		{
			return nullptr;
		}
	}
	
	std::size_t new_size = size + current_size;
	
	// Ensure the memory region hasn't been exhausted.
	if (new_size > max_size)
	{
		push_exception(exception::out_of_memory);
		
		return nullptr;
	}
	
	// Calculate the pages necessary for the current and new region sizes.
	std::size_t current_page_count =
		current_size == 0 ? 0 : math::multiple_of(current_size, get_page_size());
	std::size_t new_page_count =
		new_size == 0 ? 0 : math::multiple_of(new_size, get_page_size());
	
	// If necessary, commit the new pages.
	if (new_page_count > current_page_count)
	{
		if (!commit_pages(memory, current_page_count, new_page_count - current_page_count))
		{
			// The memory region could not grow for any variety of reasons.
			return nullptr;
		}
	}
	
	// Adjust the current size with the new data to track how much the memory
	// region has grown on success, to keep the object in a valid state.
	current_size = new_size;
	
	// As per the requirements, return the pointer to the beginning of the region.
	// Objects using the memory region should know the region grows linearly;
	// thus, to calculate the position of any newly allocated data, simply advance
	// 'new_size' bytes.
	return memory;
}

void holo::memory_region::reset(bool release)
{
	// This is a no-op if memory hasn't yet been reserved.
	if (memory != nullptr)
	{
		// There's no need to decommit pages if they none have been comitted yet.
		if (current_size > 0)
		{
			std::size_t current_page_count = math::multiple_of(current_size, get_page_size());
			
			decommit_pages(memory, 0, current_page_count);
			
			// Reset the current size.
			current_size = 0;
		}
		
		// This method only releases the region of memory if necessary (i.e., 
		// 'release' is true); otherwise, memory is just deallocated (see above).
		if (release)
		{
			release_pages(memory, 0, get_reserved_size() / get_page_size());
			memory = nullptr;
		}
	}
}

std::size_t holo::memory_region::get_reserved_size() const
{
	// If 'max_size' is 0, this is an empty memory region. Simply return 0 in such
	// a case, since an empty memory region is perfectly valid.
	if (max_size == 0)
	{
		return 0;
	}
	
	return math::round_up(max_size, get_page_size());
}

std::size_t holo::memory_region::get_current_size() const
{
	// No memory has been allocated, and thus the region has no size.
	if (current_size == 0)
	{
		return 0;
	}
	
	// Otherwise, pages have been committed. As per the purpose of this method,
	// return the number of pages committed as a size in bytes.
	return math::round_up(current_size, get_page_size());
}
