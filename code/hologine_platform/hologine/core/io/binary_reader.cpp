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
#include "core/io/binary_reader.hpp"
#include "core/io/endianness.hpp"
#include "core/io/stream_interface.hpp"
#include "core/math/bits.hpp"

holo::binary_reader::binary_reader(holo::stream_interface* stream, int endianness) :
	backing_stream(stream)
{
	holo_assert(stream && stream->get_readable());
	
#ifdef HOLOGINE_LITTLE_ENDIAN
	swap = (endianness != endianness::little);
#elif HOLOGINE_BIG_ENDIAN
	swap = (endianness != endianness::big);
#endif
}

bool holo::binary_reader::read_byte(std::int8_t& value)
{
	// Read as much as we can of the data type from the backing stream into the
	// passed primitive. We don't have to swap bytes because there is only one,
	// so we can just return the result value of the read operation directly.
	return (backing_stream->read((std::uint8_t*)&value, sizeof(std::int8_t)) == sizeof(std::int8_t));	
}

bool holo::binary_reader::read_ubyte(std::uint8_t& value)
{
	// Read as much as we can of the data type from the backing stream into the
	// passed primitive. We don't have to swap bytes because there is only one,
	// so we can just return the result value of the read operation directly.
	return (backing_stream->read(&value, sizeof(std::uint8_t)) == sizeof(std::uint8_t));
}

bool holo::binary_reader::read_short(std::int16_t& value)
{
	// Read as much as we can of the data type from the backing stream into the
	// passed primitive, returning if the read operation indicates a partial
	// operation.
	if (backing_stream->read((std::uint8_t*)&value, sizeof(std::int16_t)) < sizeof(std::int16_t))
		return false;
	
	// Swap the bytes of the primitive if the endianness of the system and the
	// holo::binary_reader do not match.
	if (swap)
		value = (std::int16_t)math::swap_bytes((std::uint16_t)value);
	
	return true;
}

bool holo::binary_reader::read_ushort(std::uint16_t& value)
{
	// Read as much as we can of the data type from the backing stream into the
	// passed primitive, returning if the read operation indicates a partial
	// operation.
	if (backing_stream->read((std::uint8_t*)&value, sizeof(std::uint16_t)) < sizeof(std::uint16_t))
		return false;
	
	// Swap the bytes of the primitive if the endianness of the system and the
	// holo::binary_reader do not match.
	if (swap)
		value = math::swap_bytes(value);
	
	return true;
}

bool holo::binary_reader::read_int(std::int32_t& value)
{
	// Read as much as we can of the data type from the backing stream into the
	// passed primitive, returning if the read operation indicates a partial
	// operation.
	if (backing_stream->read((std::uint8_t*)&value, sizeof(std::int32_t)) < sizeof(std::int32_t))
		return false;
	
	// Swap the bytes of the primitive if the endianness of the system and the
	// holo::binary_reader do not match.
	if (swap)
		value = (std::int32_t)math::swap_bytes((std::uint32_t)value);
	
	return true;
}

bool holo::binary_reader::read_uint(std::uint32_t& value)
{
	// Read as much as we can of the data type from the backing stream into the
	// passed primitive, returning if the read operation indicates a partial
	// operation.
	if (backing_stream->read((std::uint8_t*)&value, sizeof(std::uint32_t)) < sizeof(std::uint32_t))
		return false;
	
	// Swap the bytes of the primitive if the endianness of the system and the
	// holo::binary_reader do not match.
	if (swap)
		value = math::swap_bytes(value);
	
	return true;
}

bool holo::binary_reader::read_long(std::int64_t& value)
{
	// Read as much as we can of the data type from the backing stream into the
	// passed primitive, returning if the read operation indicates a partial
	// operation.
	if (backing_stream->read((std::uint8_t*)&value, sizeof(std::int64_t)) < sizeof(std::int64_t))
		return false;
	
	// Swap the bytes of the primitive if the endianness of the system and the
	// holo::binary_reader do not match.
	if (swap)
		value = (std::int64_t)math::swap_bytes((std::uint64_t)value);
	
	return true;
}

bool holo::binary_reader::read_ulong(std::uint64_t& value)
{
	// Read as much as we can of the data type from the backing stream into the
	// passed primitive, returning if the read operation indicates a partial
	// operation.
	if (backing_stream->read((std::uint8_t*)&value, sizeof(std::uint64_t)) < sizeof(std::uint64_t))
		return false;
	
	// Swap the bytes of the primitive if the endianness of the system and the
	// holo::binary_reader do not match.
	if (swap)
		value = math::swap_bytes(value);
	
	return true;
}

holo::stream_interface* holo::binary_reader::get_stream() const
{
	return backing_stream;
}
