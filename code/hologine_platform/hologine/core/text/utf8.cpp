// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include <cstdlib>
#include "core/exception.hpp"
#include "core/text/unicode.hpp"
#include "core/text/utf8.hpp"

bool holo::unicode::is_utf8_trail_byte(utf8_char c)
{
	// Keep in mind, for all trailing bytes, the last bit is 1 and the second
	// to last is 0 (the last bit is considered the most significant bit in this
	// case).
	return ((c >> 6) == 2);
}

holo::exception_code holo::unicode::get_utf8_sequence_length(utf8_char lead, std::size_t& length)
{
	if (lead < 0x80)
	{
		// A simple ASCII sequence; UTF-8 is a superset of ASCII, which means all
		// valid ASCII characters can be used without problem. Any other lead
		// indicates a multibyte sequence, only a handful of which are valid.
		length = 1;
	}
	else if ((lead >> 5) == 0x06)
	{
		length = 2;
	}
	else if ((lead >> 4) == 0x0e)
	{
		length = 3;
	}
	else if ((lead >> 3) == 0x1e)
	{
		length = 4;
	}
	else
	{
		length = 0;

		// Invalid lead.
		return holo::exception::invalid_unicode_lead;
	}

	return holo::exception::none;
}

std::size_t holo::unicode::get_code_point_utf8_sequence_length(holo::unicode::code_point code_point)
{
	if (code_point < 0x80)
	{
		return 1;
	}
	else if (code_point < 0x800)
	{
		return 2;
	}
	else if (code_point < 0x10000)
	{
		return 3;
	}
	else
	{
		// Querying if the code point is valid should be performed before querying
		// the length of it; therefore, just assume the code point is valid, and
		// thus it is 4 bytes long.
		return 4;
	}
}

holo::exception_code holo::unicode::utf8_increment(
	const char* string,
	std::size_t length,
	std::size_t& offset,
	holo::unicode::code_point& value)
{
	std::size_t sequence_length;
	holo::exception_code exception = get_utf8_sequence_length((utf8_char)string[offset], sequence_length);

	if (exception == holo::exception::none)
	{
		std::size_t new_offset = offset + sequence_length;

		// Two verifications must be done.
		// We need to ensure the new offset is not out of bounds... and, very
		// importantly, we need to make sure that 'offset' did not wrap.
		if (new_offset <= length && new_offset > offset)
		{
			// So we don't have to cast when we index the string.
			utf8_char* utf8_string = (utf8_char*)string;

			for (std::size_t i = 1; i < sequence_length; ++i)
			{
				// Verify that this is a trailing byte.
				if (!is_utf8_trail_byte(utf8_string[offset + i]))
				{
					// offset is not adjusted because we don't know the location of the
					// next valid character. The iterator, or whatever else, should
					// handle such a situation by preventing iteration after an
					// error is detected.
					return holo::exception::incomplete_unicode_sequence;
				}
			}

			// UTF-8 sequences are stored in the byte stream from most significant
			// to least significant. There are a different number of bits available
			// between 1, 2, 3, and 4 length sequences in the lead byte. Trailing
			// bytes provide an additional 6 bits per.
			//
			// When noting 'first of x bits' or 'last of x bits', the order is from
			// least significant to most significant; thus, saying 'the last 5 bits'
			// would mean any of (in an 8 bit number) xxxxx000, while the first 3 bits
			// would be 00000xxx.
			switch (sequence_length)
			{
				case 1:
					// It's just an ASCII character. No bit twiddling necessary.
					value = utf8_string[offset];
					break;

				case 2:
					// The lead byte has the last 5 bits of a 11 bit sequence.
					value = (utf8_string[offset] & 0x1f) << 6;

					// The relevant bits of the second byte are stored in the first 6 bits
					// of the code point.
					value |= utf8_string[offset + 1] & 0x3f;
					break;

				case 3:
					// The lead byte has the last 4 bits of a 16 bit sequence.
					value = (utf8_string[offset] & 0x0f) << 12;

					// The relevant bits of the second byte are stored in bits 7 to 12 of
					// the code point.
					value |= (utf8_string[offset + 1] & 0x3f) << 6;

					// The relevant bits of the last byte are stored in the first 6 bits of
					// the code point.
					value |= utf8_string[offset + 2] & 0x3f;
					break;

				case 4:
					// The lead byte has the last 3 bits of a 21 bit sequence.
					value = utf8_string[offset] & 0x07 << 18;

					// The relevant bits of the second byte are stored in bits 12 to 18 of
					// the code point.
					value |= (utf8_string[offset + 1] & 0x3f) << 12;

					// The relevant bits of the third byte are stored in bits 6 to 11 of
					// the code point.
					value |= (utf8_string[offset + 1] & 0x3f) << 6;

					// The relevant bits of the last byte are stored in the first 6 bits of
					// the code point.
					value |= utf8_string[offset + 3] & 0x3f;
					break;
			}

			// An overlong sequence is considered invalid and a security risk.
			if (get_code_point_utf8_sequence_length(value) != sequence_length)
				return holo::exception::overlong_unicode_sequence;

			// Similarly, an invalid code point is no good.
			if (!is_valid_code_point(value))
				return holo::exception::invalid_unicode_lead;

			// We don't adjust offset in the event of an error, because there is no
			// way of reliably knowing the next valid input. At best, we store the
			// invalid code point in the iterator and return an exception.
			offset = new_offset;
		}
		else
		{
			// Either an offset out of bounds or a wrapped length are cause for
			// concern. Return a relevant exception.
			return holo::exception::index_out_of_range;
		}
	}

	return exception;
}


holo::exception_code holo::unicode::utf8_decrement(
	const char* string,
	std::size_t& offset,
	holo::unicode::code_point& value)
{
	// Cannot move before the beginning of the string! That's a classic
	// buffer overflow. In this case, size_t would wrap.
	if (offset == 0)
		return holo::exception::index_out_of_range;

	// We use holo::unicode::utf8_increment(const char*, size_t, size_t&, code_point&)
	// to retrieve the actual code point. However, this decrement counterpart
	// is not provided the length of the string. Therefore, we must calculate a
	// sensible 'length'.
	//
	// Since the input is required to be aligned along a UTF-8 sequence, and we
	// are trying to retrieve the code point from the previous sequence, simply
	// store the initial offset as the length.
	std::size_t length = offset;

	// This method should not alter 'offset' unless the validity of the input so
	// far can be ensured. Operate on a copy for now.
	std::size_t current_offset = offset;

	do
	{
		--current_offset;
	} while (current_offset > 0 && is_utf8_trail_byte(string[current_offset]));

	// We're at the previous sequence, so increment to retrieve the code point.
	// Of course, we want the offset to be at the beginning of the previous
	// sequence--so passing 'offset' naively will result in 'offset' point to the
	// current sequence again, which is not the intended behavior.
	//
	// Therefore, we must store the offset in a temporary variable, so when
	// holo::unicode::utf8_increment(const char*, size_t, size_t&, code_point&)
	// adjusts the 'offset', it's not propagated up.
	std::size_t final_offset = current_offset;

	// Retrieve the character and only update 'offset' on success.
	holo::exception_code exception = 
		utf8_increment(string, length, final_offset, value);

	if (exception == holo::exception::none)
	{
		offset = current_offset;
	}

	return exception;
}