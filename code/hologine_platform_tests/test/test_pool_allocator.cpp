// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include <boost/test/unit_test.hpp>
#include "core/memory/memory_arena_pool.hpp"
#include "core/memory/pool_allocator.hpp"

namespace config
{
	const static std::size_t arena_size = 0x8000u;
	const static std::size_t arena_count = 4;
	const static std::size_t pool_object_size = 0x200u;
}

struct pool_allocator_test
{
	pool_allocator_test();
	~pool_allocator_test();

	holo::memory_arena_pool arena_pool;
	holo::pool_allocator allocator;
};

pool_allocator_test::pool_allocator_test() :
	arena_pool(config::arena_size, config::arena_count),
	allocator(&arena_pool, config::pool_object_size)
{
	// Nothing.
}

pool_allocator_test::~pool_allocator_test()
{
	// Nothing.
}

BOOST_FIXTURE_TEST_SUITE(pool_allocator_test_suite, pool_allocator_test)

BOOST_AUTO_TEST_CASE(object_size_valid)
{
	BOOST_REQUIRE(allocator.get_object_size() >= config::pool_object_size);
}

BOOST_AUTO_TEST_CASE(single_allocation_deallocation)
{
	void* pointer = allocator.allocate(0x100u);
	holo::memory_arena_pool::arena_record* record = arena_pool.get_arena(pointer);

	// Valid calls to allocate should return valid pointers.
	//
	// If the test system is out of memory, this will fail. Oh well.
	BOOST_REQUIRE(pointer != nullptr);

	// An arena should be reserved. Since the pool allocator is the only user of
	// the arena pool, only one arena should have been allocated.
	BOOST_REQUIRE(arena_pool.get_arena_count() == 1);

	// The arena should have properly been updated to reflect its current owner.
	BOOST_REQUIRE(record->allocator == &allocator);

	// One free node should have been reserved in the arena.
	BOOST_REQUIRE(record->free_node_count == allocator.get_object_count() - 1);

	allocator.deallocate(pointer);

	// The pool allocator should return unused arenas.
	BOOST_REQUIRE(record->allocator == nullptr);
}

BOOST_AUTO_TEST_CASE(requesting_new_memory_region)
{
	std::size_t max = allocator.get_object_count();

	for (std::size_t i = 0; i < max; ++i)
	{
		allocator.allocate(0x200u);
	}

	// Only one arnea should be required per 'max' allocations.
	BOOST_REQUIRE(arena_pool.get_arena_count() == 1);

	allocator.allocate(1);

	// Once 'max' is exceeded, a new arena should be allocated for a further 'max'
	// allocations.
	BOOST_REQUIRE(arena_pool.get_arena_count() == 2);
}

BOOST_AUTO_TEST_SUITE_END()
