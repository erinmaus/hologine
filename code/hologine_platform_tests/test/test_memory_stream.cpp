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
#include <cstdint>
#include <cstring>
#include "core/io/memory_stream.hpp"

namespace config
{
	const static std::size_t buffer_size = 0x200u;
}

struct memory_stream_test
{
	memory_stream_test();
	~memory_stream_test();

	std::uint8_t buffer[config::buffer_size];
	holo::memory_stream stream;
};

memory_stream_test::memory_stream_test() :
	stream(buffer, config::buffer_size)
{
	std::memset(buffer, 0, config::buffer_size);
}

memory_stream_test::~memory_stream_test()
{
	// Nothing.
}

BOOST_FIXTURE_TEST_SUITE(memory_stream_test_suite, memory_stream_test)

BOOST_AUTO_TEST_CASE(seeking)
{
	BOOST_REQUIRE(stream.get_position() == 0);
	BOOST_REQUIRE(stream.seek(0x100u, holo::seek_flags::absolute) == true);
	BOOST_REQUIRE(stream.get_position() == 0x100u);
	BOOST_REQUIRE(stream.seek(0x50u, holo::seek_flags::forward_relative) == true);
	BOOST_REQUIRE(stream.get_position() == 0x150u);
	BOOST_REQUIRE(stream.seek(0x50u, holo::seek_flags::backward_relative) == true);
	BOOST_REQUIRE(stream.get_position() == 0x100u);
	BOOST_REQUIRE(stream.seek(0x100, holo::seek_flags::end) == true);
	BOOST_REQUIRE(stream.get_position() == 0);

	BOOST_REQUIRE(stream.seek(0x10000, holo::seek_flags::absolute) == true);
	BOOST_REQUIRE(stream.get_position() == config::buffer_size);
	BOOST_REQUIRE(stream.seek(0x10000, holo::seek_flags::forward_relative) == true);
	BOOST_REQUIRE(stream.get_position() == config::buffer_size);
	BOOST_REQUIRE(stream.seek(0x100000, holo::seek_flags::backward_relative) == true);
	BOOST_REQUIRE(stream.get_position() == 0);
	BOOST_REQUIRE(stream.seek(0x100000, holo::seek_flags::end) == true);
	BOOST_REQUIRE(stream.get_position() == 0);
}

BOOST_AUTO_TEST_CASE(writing)
{
	std::uint8_t other_buffer[config::buffer_size * 2];
	for (std::size_t i = 0; i < sizeof(other_buffer); ++i)
	{
		other_buffer[i] = (std::uint8_t)(i & 0xff);
	}

	BOOST_REQUIRE(stream.write(other_buffer, sizeof(other_buffer)) == config::buffer_size);
}

BOOST_AUTO_TEST_CASE(reading)
{
	std::uint8_t other_buffer[config::buffer_size * 2];

	BOOST_REQUIRE(stream.read(other_buffer, sizeof(other_buffer)) == config::buffer_size);
}

BOOST_AUTO_TEST_SUITE_END()
