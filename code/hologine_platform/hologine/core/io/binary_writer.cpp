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
#include "core/handle.hpp"
#include "core/io/binary_writer.hpp"
#include "core/io/stream_interface.hpp"
#include "core/io/endianness.hpp"
#include "core/math/bits.hpp"

holo::binary_writer::binary_writer(holo::stream_interface* stream, int endianness) :
	backing_stream(stream)
{
	holo_assert(stream && stream->get_writable());
	
#ifdef HOLOGINE_LITTLE_ENDIAN
	swap = (endianness != endianness::little);
#elif HOLOGINE_BIG_ENDIAN
	swap = (endianness != endianness::big);
#endif
}

bool holo::binary_writer::write_byte(std::int8_t value)
{
	// Write the value to the stream.
	return (backing_stream->write((std::uint8_t*)&value, sizeof(std::int8_t)) == sizeof(std::int8_t));	
}

bool holo::binary_writer::write_ubyte(std::uint8_t value)
{
	// Write the value to the stream.
	return (backing_stream->write(&value, sizeof(std::uint8_t)) == sizeof(std::uint8_t));
}

bool holo::binary_writer::write_short(std::int16_t value)
{
	// Swap the bytes of the primitive if the endianness of the system and the
	// holo::io::binary_writer do not match.
	if (swap)
		value = (std::int16_t)math::swap_bytes((std::uint16_t)value);
	
	// Write the value to the stream.
	return (backing_stream->write((std::uint8_t*)&value, sizeof(std::int16_t)) == sizeof(std::int16_t));	
}

bool holo::binary_writer::write_ushort(std::uint16_t value)
{
	// Swap the bytes of the primitive if the endianness of the system and the
	// holo::io::binary_writer do not match.
	if (swap)
		value = math::swap_bytes(value);
	
	// Write the value to the stream.
	return (backing_stream->write((std::uint8_t*)&value, sizeof(std::uint16_t)) == sizeof(std::uint16_t));	
}

bool holo::binary_writer::write_int(std::int32_t value)
{
	// Swap the bytes of the primitive if the endianness of the system and the
	// holo::io::binary_writer do not match.
	if (swap)
		value = (std::int32_t)math::swap_bytes((std::uint32_t)value);
	
	// Write the value to the stream.
	return (backing_stream->write((std::uint8_t*)&value, sizeof(std::int32_t)) == sizeof(std::int32_t));	
}

bool holo::binary_writer::write_uint(std::uint32_t value)
{
	// Swap the bytes of the primitive if the endianness of the system and the
	// holo::io::binary_writer do not match.
	if (swap)
		value = math::swap_bytes(value);
	
	// Write the value to the stream.
	return (backing_stream->write((std::uint8_t*)&value, sizeof(std::uint32_t)) == sizeof(std::uint32_t));	
}

bool holo::binary_writer::write_long(std::int64_t value)
{
	// Swap the bytes of the primitive if the endianness of the system and the
	// holo::io::binary_writer do not match.
	if (swap)
		value = (std::int64_t)math::swap_bytes((std::uint64_t)value);
	
	// Write the value to the stream.
	return (backing_stream->write((std::uint8_t*)&value, sizeof(std::int64_t)) == sizeof(std::int64_t));	
}

bool holo::binary_writer::write_ulong(std::uint64_t value)
{
	// Swap the bytes of the primitive if the endianness of the system and the
	// holo::io::binary_writer do not match.
	if (swap)
		value = math::swap_bytes(value);
	
	// Write the value to the stream.
	return (backing_stream->write((std::uint8_t*)&value, sizeof(std::uint64_t)) == sizeof(std::uint64_t));	
}

holo::stream_interface* holo::binary_writer::get_stream() const
{
	return backing_stream;
}
