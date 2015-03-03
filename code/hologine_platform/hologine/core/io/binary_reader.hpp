// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_IO_BINARY_READER_HPP_
#define HOLOGINE_CORE_IO_BINARY_READER_HPP_

#include <cstdint>
#include "core/handle.hpp"
#include "core/io/endianness.hpp"
#include "core/io/stream_interface.hpp"

namespace holo
{
	// A reader which reads binary data from streams.
	class binary_reader
	{
		public:
			// Creates a new binary stream reader in the specified endianness,
			// utilizing the specified stream interface.
			// 
			// The possible endianness values are either
			// holo::endianness::little or holo::endianness::big.
			binary_reader(holo::stream_interface* stream, int endianness);
			
			// Reads the next signed, 8-bit integer from the stream into the user-
			// allocated primitive and advances the stream's position by the number
			// of bytes read.
			// 
			// Returns true if the data was completely read from the stream;
			// otherwise, false will be returned and the value of the specified
			// primitive will be undefined.
			bool read_byte(std::int8_t& value);
			
			// Reads the next unsigned, 8-bit integer from the stream into theuser-
			// allocated primitive and advances the stream's position by the number
			// of bytes read.
			// 
			// Returns true if the data was completely read from the stream;
			// otherwise, false will be returned and the value of the specified
			// primitive will be undefined.
			bool read_ubyte(std::uint8_t& value);
			
			// Reads the next signed, 16-bit integer from the stream into the user-
			// allocated primitive and advances the position of the stream by the
			// number of bytes read. Byte-swapping will be performed as to match the
			// system's endianness.
			// 
			// Returns true if the data was completely read from the stream;
			// otherwise, false will be returned and the value of the specified
			// primitive will be undefined.
			bool read_short(std::int16_t& value);
			
			// Reads the next unsigned, 16-bit integer from the stream into the
			// user-allocated primitive and advances the position of the stream by
			// the number of bytes read. Byte-swapping will be performed as to match
			// the system's endianness.
			// 
			// Returns true if the data was completely read from the stream;
			// otherwise, false will be returned and the value of the specified
			// primitive will be undefined.
			bool read_ushort(std::uint16_t& value);
			
			// Reads the next signed, 32-bit integer from the stream into the user-
			// allocated primitive and advances the position of the stream by the
			// number of bytes read. Byte-swapping will be performed as to match the
			// system's endianness.
			// 
			// Returns true if the data was completely read from the stream;
			// otherwise, false will be returned and the value of the specified
			// primitive will be undefined.
			bool read_int(std::int32_t& value);
			
			// Reads the next unsigned, 32-bit integer from the stream into the
			// user-allocated primitive and advances the position of the stream by
			// the number of bytes read. Byte-swapping will be performed as to match
			// the system's endianness.
			// 
			// Returns true if the data was completely read from the stream;
			// otherwise, false will be returned and the value of the specified
			// primitive will be undefined.
			bool read_uint(std::uint32_t& value);
			
			// Reads the next signed, 64-bit integer from the stream into the user-
			// allocated primitive and advances the position of the stream by the
			// number of bytes read. Byte-swapping will be performed as to match the
			// system's endianness.
			// 
			// Returns true if the data was completely read from the stream;
			// otherwise, false will be returned and the value of the specified
			// primitive will be undefined.
			bool read_long(std::int64_t& value);
			
			// Reads the next unsigned, 64-bit integer from the stream into the
			// user-allocated primitive and advances the position of the stream by
			// the number of bytes read. Byte-swapping will be performed as to match
			// the system's endianness.
			// 
			// Returns true if the data was completely read from the stream;
			// otherwise, false will be returned and the value of the specified
			// primitive will be undefined.
			bool read_ulong(std::uint64_t& value);
			
			// Reads the next holo::handle from the stream into the user- allocated
			// instance and advances the position of the stream by the number of
			// bytes read. Byte-swapping will be performed as to match the
			// holo::binary_writer's endianness.
			// 
			// Each individual field of the holo::handle (type, age, scope, and
			// index) will be read sequentially as 32-bit, unsigned integers and
			// then encoded into a single holo::handle.
			// 
			// Returns true if the data was completely written to the stream;
			// otherwise, false will be returned.
			template <class HandleDefinition>
			bool read_handle(holo::handle& handle);

			// Returns the underlying stream.
			holo::stream_interface* get_stream() const;
			
		private:
			// Whether or not the bytes in the data we read from our stream are to
			// be swapped before returning. This occurs when the system's endianness
			// does not match the endianness of the holo::binary_reader.
			bool swap;
			
			// The backing stream interface used to read from the stream.
			holo::stream_interface* backing_stream;
	};

	template <class HandleDefinition>
	bool binary_reader::read_handle(holo::handle& handle)
	{
		std::uint32_t type, age, scope, index;

		if (!read_uint(type) || !read_uint(age) || !read_uint(scope) || !read_uint(index))
		{
			return false;
		}

		if (HandleDefinition::type != type)
		{
			return false;
		}

		handle = HandleDefinition::encode(age, scope, index);

		return true;
	}
}

#endif
