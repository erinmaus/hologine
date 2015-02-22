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
#include "core/text/string.hpp"
#include "core/text/string_iterator.hpp"
#include "core/text/utf8.hpp"

holo::string_iterator::string_iterator() :
	string(nullptr),
	next_offset(0),
	current_offset(0),
	cached_code_point_value(0),
	is_valid(true)
{
	// Nothing.
}

bool holo::string_iterator::get_is_valid() const
{
	return is_valid;
}

holo::string_iterator::string_iterator(const holo::string* string, std::size_t offset) :
	string(string),
	next_offset(offset),
	current_offset(offset),
	is_valid(true)
{
	// The string must not be NULL. Since only holo::string can construct a
	// holo::string_iterator instance, this would be a terrible logic error.
	holo_assert(string);

	// Offset cannot be anything but zero for a NULL string.
	holo_assert(string->buffer != nullptr || offset == 0);

	// Consider the initial state of a holo::string_iterator to be before the
	// expected position; that is, *it == code_point_at_offset - 1. Therefore, we
	// must kickstart the state.
	//
	// Of course, only kickstart if this is not an end iterator.
	if (offset < string->length)
	{
		increment();
	}
}

holo::unicode::code_point holo::string_iterator::dereference() const
{
	return cached_code_point_value;
}

void holo::string_iterator::increment()
{
	if (string != nullptr)
	{
		// An iterator in an invalid state is unacceptable. The calling method
		// should have found a solution or bailed out.
		//
		// If we continued iterating, we could be leaking garbage data from a
		// garbage string. Play it safe--especially with strings.
		if (!is_valid)
			terminate();

		const char* buffer = nullptr;

		// Find the correct buffer.
		if (string->storage_mode == holo::string::storage_managed
			&& string->buffer != nullptr)
		{
			buffer = string->buffer->data;
		}
		else if (string->storage_mode == holo::string::storage_unmanaged)
		{
			buffer = string->const_string;
		}

		if (buffer != nullptr)
		{
			std::size_t pending_current_offset = next_offset;

			// This method will not update 'next_offset' on failure.
			holo::exception_code exception = unicode::utf8_increment(
				buffer + string->offset,
				string->length,
				next_offset,
				cached_code_point_value);

			if (exception != holo::exception::none)
			{
				is_valid = false;
				push_exception(exception);	
			}
			else
			{
				// Even though an error will put us into an invalid state logically, the
				// internal state should still be sane.
				current_offset = pending_current_offset;
			}
		}
	}
}

void holo::string_iterator::decrement()
{
	if (string != nullptr)
	{
		// Invalid iterators cannot be used (see holo::string_iterator::increment()).
		if (!is_valid)
			terminate();

		const char* buffer = nullptr;

		// Find the correct buffer.
		if (string->storage_mode == holo::string::storage_managed
			&& string->buffer != nullptr)
		{
			buffer = string->buffer->data;
		}
		else if (string->storage_mode == holo::string::storage_unmanaged)
		{
			buffer = string->const_string;
		}

		if (string->buffer != nullptr)
		{
			std::size_t pending_next_offset = current_offset;
			
			// This method will not update 'next_offset' on failure.
			holo::exception_code exception = unicode::utf8_decrement(
				buffer + string->offset,
				current_offset,
				cached_code_point_value);

			if (exception != holo::exception::none)
			{
				is_valid = false;
				push_exception(exception);
			}
			else
			{
				// Like with holo::string_iterator::increment(), logical invalid state
				// should not mean the physical state of the iterator is invalid.
				next_offset = pending_next_offset;
			}
		}
	}
}

bool holo::string_iterator::equal(const holo::string_iterator& other) const
{
	// Only compare relevant values.
	return string == other.string && next_offset == other.next_offset;
}
