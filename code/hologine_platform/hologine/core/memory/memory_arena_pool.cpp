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
#include <algorithm>
#include <cstring>
#include "core/platform.hpp"
#include "core/memory/memory_region.hpp"
#include "core/memory/memory_arena_pool.hpp"

holo::memory_arena_pool::memory_arena_pool(
	std::size_t arena_size_hint,
	std::size_t arena_count_hint) :
		memory_region(),
		records(nullptr),
		next_free_arena(nullptr),
		arena_pool(nullptr),
		arena_size(arena_size_hint),
		arena_reserved(arena_count_hint),
		arena_count(0)
{
	std::size_t record_size = std::max(
		std::max(holo::memory_region::get_page_size(), holo::memory_region::get_granularity()),
		arena_count_hint * sizeof(arena_record));
	std::size_t arena_span_size = arena_size_hint * arena_count_hint;
	memory_region = std::move(holo::memory_region(record_size + arena_span_size));

	void* base_pointer = memory_region.grow(record_size);
	if (base_pointer != nullptr)
	{
		records = (arena_record*)base_pointer;
		std::memset(records, 0, record_size);

		arena_pool = (char*)base_pointer + record_size;
	}
}

holo::memory_arena_pool::~memory_arena_pool()
{
	// Nothing.
}

std::size_t holo::memory_arena_pool::reserve(std::size_t count)
{
	std::size_t reserved = 0;
	while (reserved < count)
	{
		if (!allocate_arena())
			break;

		++reserved;
	}

	return reserved;
}

holo::memory_arena_pool::arena_record* holo::memory_arena_pool::take_arena()
{
	arena_record* record = nullptr;
	
	if (next_free_arena == nullptr && !allocate_arena())
	{
		// We couldn't allocate a new region for some reason.
		return nullptr;
	}
	else
	{
		record = next_free_arena;
		
		// Update the head of the free list.
		next_free_arena = record->next;
	}

	return record;
}

void holo::memory_arena_pool::give_arena(arena_record* record)
{
	record->allocator = nullptr;
	record->free_node_count = 0;
	record->free_node_list = nullptr;
	record->previous = nullptr;

	if (next_free_arena)
	{
		record->next = next_free_arena;
	}

	next_free_arena = record;
}

holo::memory_arena_pool::arena_record* holo::memory_arena_pool::get_arena(void* pointer)
{
	void* begin = arena_pool;
	void* end = (char*)arena_pool + arena_count * arena_size;

	if (pointer >= begin && pointer < end)
	{
		return &records[((holo::unsigned_pointer)pointer - (holo::unsigned_pointer)begin) / arena_size];
	}

	return nullptr;
}

std::size_t holo::memory_arena_pool::get_arena_size() const
{
	return arena_size;
}

std::size_t holo::memory_arena_pool::get_arena_count() const
{
	return arena_count;
}

std::size_t holo::memory_arena_pool::get_reserved_arena_count() const
{
	return arena_reserved;
}

bool holo::memory_arena_pool::allocate_arena()
{
	if (arena_count < arena_reserved)
	{
		void* base_pointer = memory_region.grow(arena_size);

		if (base_pointer)
		{
			records[arena_count].base = (char*)arena_pool + arena_count * arena_size;

			records[arena_count].next = next_free_arena;
			next_free_arena = &records[arena_count];

			++arena_count;

			return true;
		}
	}

	return false;
}
