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
#include "core/text/unicode.hpp"

bool holo::unicode::is_valid_code_point(holo::unicode::code_point code_point)
{
	// The maximum unicode code point is 0x0010ffff.
	// Surrogates are in the range of 0xd800 and 0xdfff, which makes them invalid
	// code points for the purpose of text.
	return code_point <= 0x0010ffff && !(code_point >= 0xd800 && code_point <= 0xdfff);
}

const holo::exception_code holo::exception::invalid_unicode_lead =
	holo::exception_code_generator::generate_exception_code("holo_exception_invalid_unicode_lead");
const holo::exception_code holo::exception::incomplete_unicode_sequence =
	holo::exception_code_generator::generate_exception_code("holo_exception_incomplete_unicode_sequence");
const holo::exception_code holo::exception::overlong_unicode_sequence =
	holo::exception_code_generator::generate_exception_code("holo_exception_overlong_unicode_sequence");
const holo::exception_code holo::exception::invalid_unicode_code_point =
	holo::exception_code_generator::generate_exception_code("holo_exception_invalid_unicode_code_point");
