// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_IO_MEMORY_STREAM_HPP_
#define HOLOGINE_CORE_IO_MEMORY_STREAM_HPP_

#include <cstdint>
#include "core/io/stream_interface.hpp"

namespace holo
{
	// Stream reader and/or writer that encapsulates raw memory blocks.
	class memory_stream : public stream_interface
	{
		public:
			// Creates a read/write memory stream encapsulating a memory
			// block of the specified size.
			memory_stream(std::uint8_t* stream, std::uint64_t length);
			
			// Creates a memory stream encapsulating a memory block of the
			// specified size with the specified read/write access.
			memory_stream(std::uint8_t* stream, std::uint64_t length, bool readable, bool writable);
			
			// Implementation.
			std::size_t read(std::uint8_t* data, std::size_t count);
			
			// Implementation.
			std::size_t write(std::uint8_t* data, std::size_t count);
			
			// Implementation.
			bool seek(std::uint64_t offset, int flags);
			
			// Implementation.
			std::uint64_t get_position() const;
			
			// Implementation.
			bool get_readable() const;
			
			// Implementation.
			bool get_writable() const;
			
			// Implementation.
			std::uint64_t get_size() const;
			
		private:
			// Whether or not we can read data.
			bool is_readable;
			
			// Whether or not we can write data.
			bool is_writable;
			
			// Our current position into the stream.
			std::uint64_t current_position;
			
			// The size of our stream.
			std::uint64_t stream_length;
			
			// The contents of our stream.
			std::uint8_t* stream_data;
	};
}

#endif
