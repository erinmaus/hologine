// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_IO_STREAM_INTERFACE_HPP_
#define HOLOGINE_CORE_IO_STREAM_INTERFACE_HPP_

#include <cstdint>

namespace holo
{
	// Base interface for data stream types.
	class stream_interface
	{
		public:
			// Reads the specified number of bytes from the stream (from its
			// current position) into the user-allocated memory block and
			// seeks ahead by the number of bytes read.
			// 
			// If the stream implementation does not support reading (i.e.
			// get_readable returns false), no data will be read and zero
			// will be returned.
			// 
			// Returns the number of bytes read from the stream. A value not
			// equal to count indicates a partial operation.
			virtual std::size_t read(std::uint8_t* data, std::size_t count) = 0;
			
			// Writes the specified number of bytes from the user-allocated
			// memory block into the stream at its current position and
			// seeks ahead by the number of bytes written.
			// 
			// If the stream implementation does not support writing (i.e.
			// get_writable returns false), no data will be written and
			// zero will be returned.
			// 
			// Returns the number of bytes read from the stream. A value not
			// equal to count indicates a partial operation.
			virtual std::size_t write(std::uint8_t* data, std::size_t count) = 0;
			
			// Steps ahead the specified number of bytes in the stream.
			// Valid flag enums are in the holo::seek_flags namespace.
			// 
			// Returns true if the seek was successful; otherwise, false.
			virtual bool seek(std::uint64_t offset, int flags) = 0;
			
			// Gets the current position in the stream, in bytes.
			virtual std::uint64_t get_position() const = 0;
			
			// Returns true if the stream implementation can read data;
			// otherwise, false.
			virtual bool get_readable() const = 0;
			
			// Returns true if the stream implementation can write data;
			// otherwise, false.
			virtual bool get_writable() const = 0;
			
			// Returns the size, in bytes, of the stream. If the stream has
			// no determinable size, zero is returned.
			virtual std::uint64_t get_length() const = 0;
	};
	
	// Flags for the holo::stream_interface::seek method.
	namespace seek_flags
	{
		enum
		{
			// Uses the seeking offset as an offset to the current position,
			// rather than an absolute one.
			relative = 0x0,
			
			// Uses the seeking offset as an absolute position, rather than
			// a relative one.
			absolute = 0x1,
		};
	}
}

#endif
