// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include <cstring>
#include "core/exception.hpp"
#include "core/memory/allocator.hpp"
#include "core/text/string.hpp"
#include "core/text/string_iterator.hpp"

holo::string::string() :
	storage_mode(storage_unmanaged),
	const_string(nullptr),
	offset(0),
	length(0)
{
	// Nothing.
}

holo::string::string(const char* data) :
	storage_mode(storage_unmanaged),
	const_string(data),
	offset(0),
	length(std::strlen(data))
{
	// Nothing.
}

holo::string::string(holo::allocator* allocator, const char* other) :
	storage_mode(storage_managed),
	offset(0)
{
	buffer = construct_buffer(allocator, other, other + std::strlen(other));
	length = buffer->length;
}

holo::string::string(const holo::string& other) :
	storage_mode(other.storage_mode),
	offset(other.offset),
	length(other.length)
{
	// Only holo::string should modify the storage mode. Regardless, ensure it is
	// in a valid range.
	holo_assert(storage_mode == storage_managed || storage_mode == storage_unmanaged);

	if (storage_mode == storage_managed)
	{
		buffer = other.buffer;

		if (buffer != nullptr)
		{
			reference_buffer(buffer);
		}
	}
	else
	{
		const_string = other.const_string;
	}
}

holo::string::~string()
{
	if (storage_mode == storage_managed && buffer != nullptr)
	{
		dereference_buffer(buffer);
	}
}

holo::string_iterator holo::string::begin() const
{
	return holo::string_iterator(this, 0);
}

holo::string_iterator holo::string::end() const
{
	return holo::string_iterator(this, length);
}

holo::string holo::string::substring(holo::string_iterator first, holo::string_iterator last) const
{
	if (first.string != this || last.string != this)
	{
		push_exception(holo::exception::invalid_argument);

		return holo::string();
	}

	// Create the substring by creating a copy of this string and then simply
	// updating the offset and length members.
	holo::string s(*this);

	s.offset = first.current_offset;
	s.length = last.current_offset - first.current_offset;

	// Return!
	return s;
}

holo::string holo::string::duplicate() const
{
	// If this string is empty, just return an empty string.
	//
	// This case covers the possible situation where 'buffer' is valid but the
	// string is empty, and also where 'buffer' is NULL (and therefore the string
	// is also empty).
	if (length == 0)
		return holo::string();

	// Only duplicate if the string object manages the internal character data.
	//
	// Without an allocator, we cannot create a copy of the underlying character
	// data. If the user wishes to ensure the string is a duplicate, the other
	// overload should be called.
	if (storage_mode == storage_unmanaged)
		return *this;

	// Otherwise, delegate the task to the other duplicate overload.
	return duplicate(buffer->allocator);
}

holo::string holo::string::duplicate(holo::allocator* allocator) const
{
	// If the string is empty, just return.
	if (length == 0)
		return holo::string();

	// Get the character data that should be duplicated. Depending on managed or
	// unmanaged strings, we'll have to select the right buffer.
	const char* data;
	if (storage_mode == storage_managed)
	{
		return buffer->data;
	}
	else
	{
		data = const_string;
	}

	// Just invoke the range constructor, tweaking the parameters, on the
	// character data.
	return holo::string(allocator, data + offset, data + offset + length);
}

holo::string& holo::string::operator =(const holo::string& other)
{
	if (this != &other)
	{
		offset = other.offset;
		length = other.length;

		if (storage_mode == storage_managed)
		{
			// We're replacing the current buffer.
			//
			// If this were the same instance as 'other' the buffer could be
			// deallocated at this point; thus, when we go to reference the 'new'
			// buffer, we'd be accessing unallocated memory!
			dereference_buffer(buffer);
		}

		if (other.storage_mode == storage_managed)
		{
			buffer = other.buffer;

			// We're referencing a new buffer.
			reference_buffer(buffer);
		}
		else
		{
			const_string = other.const_string;
		}

		storage_mode = other.storage_mode;
	}

	return *this;
}

void holo::string::reference_buffer(string_buffer* buffer)
{
	++buffer->reference_count;
}

void holo::string::dereference_buffer(string_buffer* buffer)
{
	// holo::string::reference_buffer(string_buffer*) and
	// holo::string:dereference_buffer(string_buffer*) are the only two objects
	// that should touch the reference count; therefore, is reference_count is 0
	// and the object still exists, it means that some other method altered the
	// state of the buffer (since this method will destroy the buffer if the
	// buffer is no longer referenced).
	holo_assert(buffer->reference_count != 0);

	if (--buffer->reference_count == 0)
	{
		buffer->allocator->deallocate(buffer->data);
		buffer->allocator->destruct(buffer);

		// Since the buffer was dereferenced for the last time, there should be no
		// pointers to the instance anymore. If they are, a fatal logic error has
		// occurred.
	}
}

bool holo::operator ==(const holo::string& left, const holo::string& right)
{
	std::size_t left_length = left.length - left.offset;
	std::size_t right_length = right.length - right.offset;

	if (left_length != right_length)
	{
		return false;
	}

	const char* left_data = nullptr;
	if (left.storage_mode == holo::string::storage_managed)
	{
		left_data = left.buffer->data;
	}
	else
	{
		left_data = left.const_string;
	}

	const char* right_data = nullptr;
	if (right.storage_mode == holo::string::storage_managed)
	{
		right_data = right.buffer->data;
	}
	else
	{
		right_data = right.const_string;
	}

	// Since left_length == right_length, we have to search each character.
	//
	// However! If left_data points to right_data, then they would be the same
	// string (since lengths are the same).
	//
	// Also, if length == 0, then both strings are empty and thus equal.
	return left_length == 0
		|| left_data == right_data
		|| std::memcmp(left_data, right_data, left_length) == 0;
}

bool holo::operator !=(const holo::string& left, const holo::string& right)
{
	return !(left == right);
}
