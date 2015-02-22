// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_TEXT_UNICODE_HPP_
#define HOLOGINE_CORE_TEXT_UNICODE_HPP_

#include "core/exception.hpp"
#include "core/platform.hpp"

namespace holo
{
	namespace unicode
	{
		// Defines a type that can store a Unicode code point.
		typedef uint32_t code_point;

		// Gets if a code point is valid.
		//
		// Returns true if the provided code point is valid, false otherwise.
		bool is_valid_code_point(uint32_t code_point);
	}

	// List of Unicode exceptions.
	//
	// Even though only UTF-8 strings and text processing are natively
	// implemented or otherwise used in Hologine, the process of encoding and
	// decoding Unicode to  multibyte formats may generate a similar set of
	// exceptions.
	namespace exception
	{
		// Represents an error in which the lead character is malformed, indicating
		// a corrupt or invalid multibyte string.
		const extern holo::exception_code invalid_unicode_lead;

		// Represents an error in which a Unicode sequence is shorter than
		// indicated by the lead character.
		const extern holo::exception_code incomplete_unicode_sequence;

		// Represents an error in which the Unicode sequence is longer than
		// indicated by the lead character, or longer than the standard allows.
		const extern holo::exception_code overlong_unicode_sequence;

		// Represents an error in which the Unicode sequence produced an invalid
		// code point.
		const extern holo::exception_code invalid_unicode_code_point;
	}
}

#endif
