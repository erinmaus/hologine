// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_MEMORY_LINEAR_ALLOCATOR_HPP_
#define HOLOGINE_CORE_MEMORY_LINEAR_ALLOCATOR_HPP_

#include "core/memory/allocator.hpp"
#include "core/memory/memory_region.hpp"

namespace holo
{
	// Increments memory linearly by simply bumping a pointer.
	//
	// A linear allocator requires a continuous region of memory. This is because
	// to allocate memory, a pointer is simply incremented by the size of the
	// allocation.
	//
	// Generally, a linear allocator is only safe to use with POD types.
	class linear_allocator final : public allocator
	{
		public:
			// Signature of a destructor passed to
			// holo::linear_allocator::set_destructor(void*, destructor_callback).
			typedef void (* destructor_callback)(void* pointer);
			
			// Constructs the linear allocator, reserving 'size' bytes at once.
			//
			// A memory region capable of storing 'size' bytes linearly will be
			// created by the linear allocator. In the event that this fails, this
			// method will push holo::exception::out_of_memory, and all further
			// operations will fail.
			//
			// Keep in mind that, like memory regions, more memory may be available
			// than requested on success. To query the final size of the linear
			// allocator, see holo::linear_allocator::get_size().
			explicit linear_allocator(std::size_t size);
			
			// Releases all resources allocated by the holo::linear
			~linear_allocator();
			
			// Allocates 'size' bytes of memory aligned to the provided boundary and
			// returns it
			//
			// The object should be simple; that is, it does not need an explicit
			// destructor.
			void* allocate(std::size_t size, std::size_t alignment = default_alignment) override;
			
			// This method does nothing. To deallocate entries, call
			// holo::linear_allocator::reset().
			void deallocate(void* pointer) override;

			// Resets the linear allocator.
			//
			// All memory previously allocated is now considered free.
			void reset();

			// Stores the state of the allocator, allowing only portions of memory
			// to be deallocated, rather than the entire region.
			//
			// Returns true on success, false on failure. Failure can occur if the
			// linear allocator has exhausted its memory region; in such an event,
			// holo::exception::out_of_memory will be pushed.
			bool push_marker();

			// Restores the state of the allocator to a previous state stored by
			// holo::linear_allocator::push_marker().
			//
			// If no previous state was pushed, this is equivalent to calling
			// holo::linear_allocator::reset().
			void pop_marker();

			// Gets the size of the underlying memory region.
			//
			// This value may differ from the requested size. If the memory region
			// was successfully created, then this value will always be greater than
			// or equal to the requested size. If creation of the memory region
			// failed, then this value will be 0.
			std::size_t get_size() const;
			
		private:
			// Memory region used to back allocations.
			//
			// The entire region will be committed on construction of the linear
			// allocator.
			holo::memory_region memory_region;
			
			// Pointer to the beginning of the memory region.
			void* memory;
			
			// Current offset into the memory region, in bytes.
			std::size_t memory_offset;

			// Location of the current marker, in bytes.
			//
			// A marker is simply an integer stored in the memory region that resets
			// 'memory_offset' to a previous state.
			//
			// If zero, this means that no marker was allocated.
			std::size_t current_marker;
	};
}

#endif
