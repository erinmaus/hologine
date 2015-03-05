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
#include "core/handle.hpp"
#include "core/io/binary_writer.hpp"
#include "core/io/memory_stream.hpp"

namespace config
{
	const static std::size_t buffer_size = 0x10;
}

struct binary_writer_test
{
	binary_writer_test();
	~binary_writer_test();

	std::uint8_t buffer[config::buffer_size];
	holo::memory_stream stream;
	holo::binary_writer writer;
};

binary_writer_test::binary_writer_test() :
	stream(buffer, config::buffer_size),
#ifdef HOLOGINE_BIG_ENDIAN
	writer(&stream, holo::endianness::big)
#elif HOLOGINE_LITTLE_ENDIAN
	writer(&stream, holo::endianness::little)
#endif
{
	std::memset(buffer, 0, config::buffer_size);
}

binary_writer_test::~binary_writer_test()
{
	// Nothing.
}

#define TEST_PRIMITIVE(type, func, value) \
	do { \
	type v = value; \
	stream.seek(0, holo::seek_flags::absolute); \
	BOOST_REQUIRE(writer.func(v) == true); \
	BOOST_REQUIRE(stream.get_position() == sizeof(type)); \
	BOOST_REQUIRE(std::memcmp(buffer, &v, sizeof(type)) == 0); \
	} while(false)

BOOST_FIXTURE_TEST_SUITE(binary_writer_test_suite, binary_writer_test)

BOOST_AUTO_TEST_CASE(byte_ubyte)
{
	TEST_PRIMITIVE(std::int8_t, write_byte, 0x10);
	TEST_PRIMITIVE(std::uint8_t, write_ubyte, 0x10);
}

BOOST_AUTO_TEST_CASE(short_ushort)
{
	TEST_PRIMITIVE(std::int16_t, write_short, 0x1020);
	TEST_PRIMITIVE(std::uint16_t, write_ushort, 0x1020);
}

BOOST_AUTO_TEST_CASE(int_uint)
{
	TEST_PRIMITIVE(std::int32_t, write_int, 0x10203040);
	TEST_PRIMITIVE(std::uint32_t, write_uint, 0x10203040);
}

BOOST_AUTO_TEST_CASE(long_ulong)
{
	TEST_PRIMITIVE(std::int64_t, write_long, 0x1020304050607080);
	TEST_PRIMITIVE(std::uint64_t, write_ulong, 0x1020304050607080);
}

BOOST_AUTO_TEST_CASE(handle)
{
	typedef holo::handle_definition<16, 20, 18, 18> test_handle;

	holo::handle value = test_handle::encode(132, 132, 132);
	BOOST_REQUIRE(writer.write_handle<test_handle>(value) == true);
	BOOST_REQUIRE(stream.get_position() == config::buffer_size);

	std::uint32_t* handle_buffer = (std::uint32_t*)buffer;
	BOOST_REQUIRE(handle_buffer[0] == 16);
	BOOST_REQUIRE(handle_buffer[1] == 132);
	BOOST_REQUIRE(handle_buffer[2] == 132);
	BOOST_REQUIRE(handle_buffer[3] == 132);
}

BOOST_AUTO_TEST_SUITE_END()
