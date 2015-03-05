// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include <algorithm>
#include "core/exception.hpp"
#include "core/math/bits.hpp"
#include "core/math/util.hpp"
#include "core/memory/heap_allocator.hpp"
#include "core/memory/pool_allocator.hpp"

holo::heap_allocator::heap_allocator(
	std::size_t arena_size,
	std::size_t arena_count,
	std::size_t pool_start,
	std::size_t pool_end) :
		memory_arena_pool(arena_size, arena_count),
		pool_allocators((holo::pool_allocator*)pool_allocators_buffer.get()),
		minimum_pool_size(math::bit_log2(pool_start)),
		maximum_pool_size(math::bit_log2(pool_end))
{
	holo_assert(pool_start > 0);
	holo_assert(pool_end >= pool_start);

	// Initialize pools. Each pool is a power of two larger than the previous.
	pool_allocator* current = pool_allocators;
	std::size_t current_size = minimum_pool_size;

	// We want to include a pool with the maximum pool size, so be inclusive.
	while (current_size <= maximum_pool_size)
	{
		// The pool allocator does not perform any allocations.
		//
		// Therefore, don't check for errors!
		new(current) holo::pool_allocator(&memory_arena_pool, 1 << current_size);

		++current_size;
		++current;
	}
}

holo::heap_allocator::~heap_allocator()
{
	std::size_t pool_count = maximum_pool_size - minimum_pool_size;
	std::size_t current_pool_index = 0;

	while (current_pool_index < pool_count)
	{
		pool_allocators[current_pool_index++].~pool_allocator();
	}
}

void* holo::heap_allocator::allocate(std::size_t size, std::size_t alignment)
{
	std::size_t minimum_alignment = std::max(alignment, default_alignment);
	std::size_t required_alignment = minimum_alignment - default_alignment;

	// 'required_alignment' is 1 byte too much for sizes > 0. Adjust if necessary.
	if (required_alignment > 0)
	{
		--required_alignment;
	}

	std::size_t final_size = required_alignment;

	// Get the clamped pool index of a pool that's large enough for this
	// allocation.
	//
	// This value is compared against maximum_pool_size to ensure there's an
	// appropriate allocator.
	std::size_t pool_index =
		std::max(math::bit_log2(final_size), minimum_pool_size);
	if (pool_index > maximum_pool_size)
	{
		push_exception(exception::out_of_memory);

		return nullptr;
	}

	// Adjust the pool index by 'minimum' size to retrieve the correct pool index
	// into the array.
	pool_index -= minimum_pool_size;

	// Perform the allocation.
	void* base_pointer = pool_allocators[pool_index].allocate(size);
	if (base_pointer == nullptr)
	{
		return nullptr;
	}

	// Align and return.
	return align_pointer(base_pointer, alignment);
}

void holo::heap_allocator::deallocate(void* pointer)
{
	auto record = memory_arena_pool.get_arena(pointer);
	holo_assert(record != nullptr);

	record->allocator->deallocate(pointer);
}
