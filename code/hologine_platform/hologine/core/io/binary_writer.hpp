// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_IO_BINARY_WRITER_HPP_
#define HOLOGINE_CORE_IO_BINARY_WRITER_HPP_

#include <cstdint>
#include "core/handle.hpp"
#include "core/io/endianness.hpp"
#include "core/io/stream_interface.hpp"

namespace holo
{
	// A writer which writes binary data to streams.
	class binary_writer
	{
		public:
			// Creates a new binary stream writer in the specified
			// endianness, utilizing the specified stream interface.
			// 
			// The possible endianness values are either
			// holo::endianness::little or holo::endianness::big.
			binary_writer(holo::stream_interface* stream, int endianness);
			
			// Writes the specified signed, 8-bit integer to the stream at
			// its current position and advances the position by the number
			// of bytes written.
			// 
			// Returns true if the data was completely written to the
			// stream; otherwise, false will be returned.
			bool write_byte(std::int8_t value);
			
			// Writes the specified unsigned, 8-bit integer to the stream at
			// its current position and advances the position by the number
			// of bytes written.
			// 
			// Returns true if the data was completely written to the
			// stream; otherwise, false will be returned.
			bool write_ubyte(std::uint8_t value);
			
			// Writes the specified signed, 16-bit integer to the stream at
			// its current position and advances the position by the number
			// of bytes written. Byte-swapping will be performed as to match
			// the holo::binary_writer's endianness.
			// 
			// Returns true if the data was completely written to the
			// stream; otherwise, false will be returned.
			bool write_short(std::int16_t value);
			
			// Writes the specified unsigned, 16-bit integer to the stream
			// at its current position and advances the position by the
			// number of bytes written. Byte-swapping will be performed as
			// to match the holo::binary_writer's endianness.
			// 
			// Returns true if the data was completely written to the
			// stream; otherwise, false will be returned.
			bool write_ushort(std::uint16_t value);
			
			// Writes the specified signed, 32-bit integer to the stream at
			// its current position and advances the position by the number
			// of bytes written. Byte-swapping will be performed as to match
			// the holo::binary_writer's endianness.
			// 
			// Returns true if the data was completely written to the
			// stream; otherwise, false will be returned.
			bool write_int(std::int32_t value);
			
			// Writes the specified unsigned, 32-bit integer to the stream
			// at its current position and advances the position by the
			// number of bytes written. Byte-swapping will be performed as
			// to match the holo::binary_writer's endianness.
			// 
			// Returns true if the data was completely written to the
			// stream; otherwise, false will be returned.
			bool write_uint(std::uint32_t value);
			
			// Writes the specified signed, 64-bit integer to the stream at
			// its current position and advances the position by the number
			// of bytes written. Byte-swapping will be performed as to match
			// the holo::binary_writer's endianness.
			// 
			// Returns true if the data was completely written to the
			// stream; otherwise, false will be returned.
			bool write_long(std::int64_t value);
			
			// Writes the specified unsigned, 64-bit integer to the stream
			// at its current position and advances the position by the
			// number of bytes written. Byte-swapping will be performed as
			// to match the holo::binary_writer's endianness.
			// 
			// Returns true if the data was completely written to the
			// stream; otherwise, false will be returned.
			bool write_ulong(std::uint64_t value);
			
			// Writes the specified holo::handle to the stream at its
			// current position and advancs the position by the number of
			// bytes written. Byte-swapping will be performed as to match
			// the holo::binary_writer's endianness.
			// 
			// Each individual field of the holo::handle (type, age, scope,
			// and index) will be written sequentially as 32-bit, unsigned
			// integers; thus, it will take up 16 bytes in the stream.
			// 
			// Returns true if the data was completely written to the
			// stream; otherwise, false will be returned.
			template <class HandleDefinition>
			bool write_handle(holo::handle handle);
			
			// Returns the underlying stream.
			holo::stream_interface* get_stream() const;
			
		private:
			// Whether or not the bytes in the data we write to our stream
			// are to be swapped before writing. This occurs when the
			// system's endianness does not match the endianness of the
			// holo::binary_writer.
			bool swap;
			
			// The backing stream interface used to read from the stream.
			holo::stream_interface* backing_stream;
	};

	template <class HandleDefinition>
	bool binary_writer::write_handle(holo::handle handle)
	{
		// Decode and write out each of the four holo::handle fields in the stream
		// as separate, 32-bit, unsigned integers, but stop as soon as any of them
		// fail (i.e. return false).
		return
		(
			write_uint((std::uint32_t)HandleDefinition::type) &&
			write_uint((std::uint32_t)HandleDefinition::decode_age(handle)) &&
			write_uint((std::uint32_t)HandleDefinition::decode_scope(handle)) &&
			write_uint((std::uint32_t)HandleDefinition::decode_index(handle))
		);
	}
}

#endif
