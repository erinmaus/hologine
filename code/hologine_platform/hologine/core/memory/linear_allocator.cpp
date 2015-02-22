// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include "core/exception.hpp"
#include "core/platform.hpp"
#include "core/math/util.hpp"
#include "core/memory/linear_allocator.hpp"

holo::linear_allocator::linear_allocator(std::size_t size) :
	memory_region(size),
	memory(nullptr),
	memory_offset(0)
{
	// Claim the entire memory region, committing all virtual memory to the
	// process; on failure, push holo::exception::out_of_memory.
	//
	// Even though an attempt to claim the memory region may push an exception, it
	// will probably be a platform exception. For the immediate purpose of linear
	// allocator, it is good enough to consider it an out of memory exception
	// because no memory is available to the linear allocator.
	memory = memory_region.claim();
	
	if (memory == nullptr)
	{
		push_exception(exception::out_of_memory);
	}
}

holo::linear_allocator::~linear_allocator()
{
	reset();
}

void* holo::linear_allocator::allocate(std::size_t size, std::size_t align)
{
	void* pointer = nullptr;
	
	// Calculate the memory offset, taking into consideration alignment.
	void* current_offset_pointer = align_pointer((char*)memory + memory_offset, align);
	std::size_t requested_memory_offset = get_pointer_distance(current_offset_pointer, memory);
	
	if (requested_memory_offset + size < get_size())
	{
		pointer = (char*)memory + requested_memory_offset;
		
		// There's enough memory available, so just bump.
		memory_offset = requested_memory_offset;
	}
	else
	{
		// Otherwise, there's no more memory. Push an exception before returning.
		push_exception(exception::out_of_memory);
	}
	
	return pointer;
}

void holo::linear_allocator::deallocate(void*)
{
	// Nothing.
	//
	// This is a linear allocator! Memory is only reclaimed by calling
	// holo::linear_allocator::reset() or the stack interface!
}

bool holo::linear_allocator::push_marker()
{
	// Setting a marker at the beginning of a linear allocator makes sense (the
	// caller should not have to know the state of the allocator), but internally,
	// it makes no sense to push the marker because of the behavior of
	// holo::linear_allocator::pop_marker() when current_marker == 0.
	//
	// When current_marker == 0, popping the marker is equivalent to resetting the
	// allocator to the initial state. Properly handling this case any other way
	// simply introduces a state-management headache.
	if (memory_offset == 0)
	{
		return true;
	}

	// Store the next marker. Note, this does not include alignment!
	// holo::linear_allocator::pop_marker() is delegated to check for alignment.
	std::size_t next_marker = memory_offset;

	// This one line snippet allocates a size_t along the proper boundary and
	// stores, invokign size_t's copy constructor, storing the current
	// current offset (of the current marker) in the new marker.
	//
	// If the allocator failed, the method will return null, which in turn will
	// make the boolean expression false. We only proceed by updating
	// 'current_marker' if the allocation was successful. Then we inform the
	// caller of success or failure.
	bool success = align_construct<std::size_t>(alignof(std::size_t), current_marker) != nullptr;

	if (success)
	{
		// Update current marker.
		current_marker = next_marker;
	}

	return success;
}

void holo::linear_allocator::pop_marker()
{
	if (memory_offset != 0)
	{
		if (current_marker == 0)
		{
			// No marker was pushed, so just reset.
			reset();
		}
		else
		{
			// Get the previous marker.
			std::size_t previous_marker = *((std::size_t*)align_pointer((char*)memory + current_marker, alignof(std::size_t)));

			// Reset the stack to the current marker location.
			memory_offset = current_marker;

			// Update the current marker.
			current_marker = previous_marker;
		}
	}
}

void holo::linear_allocator::reset()
{
	// Reset the offset to the beginning of the memory region.
	memory_offset = 0;
	current_marker = 0;
}

std::size_t holo::linear_allocator::get_size() const
{
	// Although the entire region should have been committed, let's play nice and
	// only return the 'current' size.
	return memory_region.get_current_size();
}
