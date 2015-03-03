// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include <cstdint>
#include "core/exception.hpp"
#include "core/io/memory_stream.hpp"
#include "core/io/stream_interface.hpp"

holo::memory_stream::memory_stream(std::uint8_t* stream, std::uint64_t length) :
	holo::memory_stream::memory_stream(stream, length, true, true)
{
	// Nothing.
}

holo::memory_stream::memory_stream(std::uint8_t* stream, std::uint64_t length, bool readable, bool writable) :
	is_readable(readable),
	is_writable(writable),
	current_position(0),
	stream_length(length),
	stream_data(stream)
{
	holo_assert(stream);
}

std::size_t holo::memory_stream::read(std::uint8_t* data, std::size_t count)
{
	// If we are not allowed to read from the memory block, or if the write-to
	// buffer is null, we can't read.
	if (!is_readable || data == nullptr)
		return 0;
	
	// Read bytes into the write-to buffer until we have either read them all or
	// we have reached the end of the stream.
	std::size_t bytes_read;
	for (bytes_read = 0; bytes_read < count && current_position < stream_length; ++bytes_read, ++current_position)
	{
		data[bytes_read] = *(stream_data + current_position);
	}
	
	// Return the number of bytes we managed to read.
	return bytes_read;
}

std::size_t holo::memory_stream::write(std::uint8_t* data, std::size_t count)
{
	// If we are not allowed to write to the memory block, or if the read-from
	// buffer is null, we can't read.
	if (!is_writable || data == nullptr)
		return 0;
	
	// Write bytes into the write-to buffer until we have either written them
	// all or we have reached the end of the stream.
	std::size_t bytes_written;
	for (bytes_written = 0; bytes_written < count && current_position < stream_length; ++bytes_written, ++current_position)
	{
		*(stream_data + current_position) = data[bytes_written];
	}
	
	// Return the number of bytes we managed to write.
	return bytes_written;
}

bool holo::memory_stream::seek(std::uint64_t offset, int flags)
{
	std::size_t new_position;

	// Ignore our current position if the user wants an absolute seek.
	if (flags & seek_flags::absolute)
	{
		new_position = offset;
	}
	else
	{
		new_position = current_position + offset;
	}
	
	// Bounds check on our position and make sure we don't step out of bounds.
	if (new_position >= stream_length)
		return false;
	
	// Seek and return success.
	current_position = new_position;
	return true;
}

std::uint64_t holo::memory_stream::get_position() const
{
	return current_position;
}

bool holo::memory_stream::get_readable() const
{
	return is_readable;
}

bool holo::memory_stream::get_writable() const
{
	return is_writable;
}

std::uint64_t holo::memory_stream::get_size() const
{
	return current_position;
}
