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
#include "core/container/intrusive_list.hpp"
#include "core/memory/memory_region_free_list.hpp"
#include "core/memory/pool_allocator.hpp"

struct pool_allocator_test
{
	pool_allocator_test();
	~pool_allocator_test();

	// Queries the number of memory regions in use by the pool allocator.
	std::size_t count_memory_regions();

	// Gets the total number of free objects available in the first memory region.
	//
	// Due to platform quirks, each memory region may have a slightly different
	// number of free objects available if empty.
	std::size_t get_free_object_count();

	holo::memory_region_free_list free_list;
	holo::pool_allocator allocator;
};

pool_allocator_test::pool_allocator_test() :
	free_list(0x8000u),
	allocator(&free_list, 0x200u)
{
	// Nothing.
}

pool_allocator_test::~pool_allocator_test()
{
	// Nothing.
}

std::size_t pool_allocator_test::count_memory_regions()
{
	auto current_iterator =
		holo::intrusive_list::make_iterator(allocator.memory_region_list_head);
	auto end_iterator =
		holo::intrusive_list::end<holo::pool_allocator::memory_region_header>();

	std::size_t count = 0;
	while (current_iterator != end_iterator)
	{
			++count;
			++current_iterator;
	}

	return count;
}

std::size_t pool_allocator_test::get_free_object_count()
{
	if (allocator.memory_region_list_head == nullptr)
		allocator.request_empty_memory_region();

	return allocator.memory_region_list_tail->size;
}

BOOST_FIXTURE_TEST_SUITE(pool_allocator_test_suite, pool_allocator_test)

BOOST_AUTO_TEST_CASE(object_size_valid)
{
	BOOST_REQUIRE(allocator.get_object_size() >= 0x200u);
	BOOST_REQUIRE(count_memory_regions() == 0);
}

BOOST_AUTO_TEST_CASE(single_allocation_deallocation)
{
	void* pointer = allocator.allocate(0x100u);

	BOOST_REQUIRE(pointer != nullptr);

	allocator.deallocate(pointer);

	BOOST_REQUIRE(count_memory_regions() == 1);
}

BOOST_AUTO_TEST_CASE(requesting_new_memory_region)
{
	std::size_t max = get_free_object_count();

	for (std::size_t i = 0; i < max; ++i)
	{
		allocator.allocate(1);
	}

	BOOST_REQUIRE(count_memory_regions() == 1);

	allocator.allocate(1);
	BOOST_REQUIRE(count_memory_regions() == 2);
}

BOOST_AUTO_TEST_CASE(freeing_old_memory_region)
{
	// This value is memory_region_size / object_size, which is the maximum
	// necessary storage.
	void* regions[128];

	std::size_t max = get_free_object_count();
	for (std::size_t i = 0; i < max; ++i)
	{
		regions[i] = allocator.allocate(1);
	}
	allocator.allocate(1);

	for (std::size_t i = 0; i < max; ++i)
	{
		allocator.deallocate(regions[i]);
	}

	BOOST_REQUIRE(count_memory_regions() == 1);
}

BOOST_AUTO_TEST_SUITE_END()