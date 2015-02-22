// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_TEXT_UTF8_HPP_
#define HOLOGINE_CORE_TEXT_UTF8_HPP_

#include <cstdlib>
#include "core/exception.hpp"
#include "core/platform.hpp"
#include "core/text/unicode.hpp"

namespace holo
{
	// Works on raw UTF-8 data stored in byte arrays.
	//
	// These methods are designed to be used by a holo::string_iterator object,
	// thus the interface is not as generic as it could be.
	namespace unicode
	{
		// The type of a UTF-8 character.
		//
		// A UTF-8 character is the basis of a UTF-8 sequence. Up to four
		// characters can be found in an individual sequence.
		typedef uint8_t utf8_char;

		// Gets if the provided byte is a trail byte.
		//
		// Returns true if the byte is a trail byte, false otherwise.
		bool is_utf8_trail_byte(utf8_char c);

		// Calculates the length of a sequence based on the lead byte.
		//
		// The length is stored in 'length'.
		//
		// Returns holo::exception::none on success; on failure, returns a relevant
		// exception (i.e., a Unicode exception if it is a validation error).
		holo::exception_code get_utf8_sequence_length(
			holo::unicode::utf8_char lead,
			std::size_t& length);

		// Calculates the length of the sequenced required to represent the provided
		// code point.
		//
		// This can be used to ensure that an overlong sequence was not provided
		// by comparing the sequence length to the required length.
		//
		// If the provided code point is invalid, this method returns 0.
		std::size_t get_code_point_utf8_sequence_length(holo::unicode::code_point code_point);

		// Advances 'offset' to the next UTF-8 sequence.
		//
		// This method is checked. Therefore, offset will not exceed 'length'. If
		// attempting to read the complete sequence would, in fact, exceed 'length',
		// holo::exception::out_of_range will be returned.
		//
		// The current offset into the string is expected to be aligned on a
		// sequence. An invalid sequence will cause an error value to be returned.
		//
		// The code point in the current sequence is stored in 'value'.
		//
		// Returns holo::exception::none on success; on failure, returns a relevant
		// exception (i.e., a Unicode exception if it is a validation error). If an
		// error occurs, 'offset' will be untouched, but 'value' may have an
		// invalid code point, depending on the error.
		holo::exception_code utf8_increment(
			const char* string,
			std::size_t length,
			std::size_t& offset,
			holo::unicode::code_point& value);

		// Returns to the previous position in the character stream, decrementing
		// 'offset' by the necessary distance.
		//
		// This method is checked. Therefore, if offset is zero and the previous
		// position has not been found, this method will return with a relevant
		// exception code.
		//
		// The current offset into the string is expected to be aligned on a
		// sequence. An invalid sequence will cause an error value to be returned.
		//
		// The code point in the previous sequence is stored in 'value'.
		//
		// Returns holo::exception::none on success; on failure, returns a relevant
		// exception (i.e., a Unicode exception if it is a validation error). If an
		// error occurs, 'offset' will be untouched, but 'value' may have an
		// invalid code point, depending on the error.
		holo::exception_code utf8_decrement(
			const char* string,
			std::size_t& offset,
			holo::unicode::code_point& value);
	}
}

#endif
