// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#define BOOST_TEST_MODULE module_utf8_string
#include <iterator>
#include <boost/test/unit_test.hpp>
#include "test/test_allocator.hpp"
#include "core/text/string.hpp"

// UTF-8 string sample.
//
// If this value is modified, tests that use it should be corrected.
const char* utf8_string_sample = "\xe5\x87\xb8\x28\xe2\x8a\x99\xe2\x96\x82\xe2\x8a\x99\xe2\x9c\x96\x20\x29";

// ASCII string sample.
//
// If this value is modified, tests that use it should be corrected.
const char* ascii_string_sample = "foobar";

// Casts an ASCII value to the equivalent holo::unicode::code_point.
holo::unicode::code_point to_code_point(char c)
{
	// Don't bother about checking if c is a valid ASCII character. The test is
	// tuned for specific cases. If the test conditions are wrong, then what is
	// truth?
	return (holo::unicode::code_point)c;
}

// Checks if the iterator is valid and correctly returns the expected code
// point and advances the iterator on success.
bool is_expected_code_point_forward(holo::string_iterator& i, holo::unicode::code_point e)
{
	// Check iterator validity.
	if (!i.get_is_valid())
		return false;

	// Get the current code point from the iterator.
	auto c = *i;

	// Increment the iterator.
	i++;

	// Compare code points.
	return e == c;
}

// Similar to is_expected_code_point_forward, but decrements the iterator.
bool is_expected_code_point_backward(holo::string_iterator& i, holo::unicode::code_point e)
{
	// Check iterator validity.
	if (!i.get_is_valid())
		return false;

	// Get the current code point from the iterator.
	auto c = *i;

	// Decrement the iterator.
	i--;

	// Compare code points.
	return e == c;
}

// Utility method to determine the validity of a UTF-8 string.
bool is_invalid_utf8_string(const holo::string& s)
{
	for (auto i = s.begin(), end = s.end(); i != end; ++i)
	{
		if (!i.get_is_valid())
		{
			return true;
		}
	}

	return false;
}

BOOST_AUTO_TEST_CASE(empty_string)
{
	holo::string s;

	BOOST_CHECK(s.begin() == s.end());
	BOOST_CHECK(s == "");
}

BOOST_AUTO_TEST_CASE(managed_unmanaged_equality_comparison)
{
	test_allocator allocator;
	holo::string s1(&allocator, "foo");
	holo::string s2("foo");

	BOOST_REQUIRE(s1 == s2);
	BOOST_REQUIRE(!(s1 != s2));
}

BOOST_AUTO_TEST_CASE(managed_unmanaged_inequality_comparison)
{
	test_allocator allocator;
	holo::string s1(&allocator, "foo");
	holo::string s2("bar");

	BOOST_REQUIRE(s1 != s2);
}

BOOST_AUTO_TEST_CASE(unmanaged_iterator)
{
	holo::string s = ascii_string_sample;
	auto iterator = s.begin();

	BOOST_REQUIRE(is_expected_code_point_forward(iterator, to_code_point('f')));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, to_code_point('o')));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, to_code_point('o')));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, to_code_point('b')));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, to_code_point('a')));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, to_code_point('r')));
	BOOST_REQUIRE(iterator == s.end());
}

BOOST_AUTO_TEST_CASE(managed_iterator)
{
	test_allocator allocator;
	holo::string s(&allocator, ascii_string_sample);
	auto iterator = s.begin();

	BOOST_REQUIRE(is_expected_code_point_forward(iterator, to_code_point('f')));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, to_code_point('o')));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, to_code_point('o')));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, to_code_point('b')));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, to_code_point('a')));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, to_code_point('r')));
	BOOST_REQUIRE(iterator == s.end());
}

BOOST_AUTO_TEST_CASE(utf8_string_iterator)
{
	holo::string s = utf8_string_sample;
	auto iterator = s.begin();

	BOOST_REQUIRE(is_expected_code_point_forward(iterator, 0x51f8u));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, 0x0028u));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, 0x2299u));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, 0x2582u));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, 0x2299u));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, 0x2716u));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, 0x0020u));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, 0x0029u));
	BOOST_REQUIRE(iterator == s.end());
}

BOOST_AUTO_TEST_CASE(utf8_string_iterator_backwards)
{
	holo::string s = utf8_string_sample;
	auto iterator = --s.end();

	BOOST_REQUIRE(is_expected_code_point_backward(iterator, 0x0029u));
	BOOST_REQUIRE(is_expected_code_point_backward(iterator, 0x0020u));
	BOOST_REQUIRE(is_expected_code_point_backward(iterator, 0x2716u));
	BOOST_REQUIRE(is_expected_code_point_backward(iterator, 0x2299u));
	BOOST_REQUIRE(is_expected_code_point_backward(iterator, 0x2582u));
	BOOST_REQUIRE(is_expected_code_point_backward(iterator, 0x2299u));
	BOOST_REQUIRE(is_expected_code_point_backward(iterator, 0x0028u));
	BOOST_REQUIRE(is_expected_code_point_backward(iterator, 0x51f8u));
	BOOST_REQUIRE(iterator == s.begin());
}

BOOST_AUTO_TEST_CASE(utf8_substring)
{
	holo::string s = utf8_string_sample;
	auto start = s.begin();
	auto stop = s.end();

	// std::advance is incompatible with holo::string_iterator for negative
	// values of 'n' despite holo::string_iterator being a bidirectional iterator?
	// Something to do with traversal vs iterator and the type returned when
	// dereferencing according to a Boost mailing list post.
	//
	// So just manually do it. And for consistency's sake, do it the same between
	// the iterators.
	for (int i = 0; i < 2; i++)
		++start;

	for (int j = 0; j < 2; j++)
		--stop;

	holo::string s_substring = s.substring(start, stop);
	auto iterator = s_substring.begin();

	BOOST_REQUIRE(is_expected_code_point_forward(iterator, 0x2299u));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, 0x2582u));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, 0x2299u));
	BOOST_REQUIRE(is_expected_code_point_forward(iterator, 0x2716u));
	BOOST_REQUIRE(iterator == s_substring.end());
}

// Taken from comment on "Pattern Modifiers" in the PHP manual.
// Current link as of 2015/2/21: http://php.net/manual/en/reference.pcre.pattern.modifiers.php#54805
BOOST_AUTO_TEST_CASE(invalid_utf8_string_iterator)
{
	// Valid ASCII 
	BOOST_REQUIRE(!is_invalid_utf8_string("a"));

	// Valid 2 Octet Sequence 
	BOOST_REQUIRE(!is_invalid_utf8_string("\xc3\xb1"));

	// Invalid 2 Octet Sequence 
	BOOST_REQUIRE(is_invalid_utf8_string("\xc3\x28"));

	// Invalid Sequence Identifier 
	BOOST_REQUIRE(is_invalid_utf8_string("\xa0\xa1"));

	// Valid 3 Octet Sequence 
	BOOST_REQUIRE(!is_invalid_utf8_string("\xe2\x82\xa1"));

	// Invalid 3 Octet Sequence (in 2nd Octet) 
	BOOST_REQUIRE(is_invalid_utf8_string("\xe2\x28\xa1"));

	// Invalid 3 Octet Sequence (in 3rd Octet) 
	BOOST_REQUIRE(is_invalid_utf8_string("\xe2\x82\x28"));

	// Valid 4 Octet Sequence 
	BOOST_REQUIRE(!is_invalid_utf8_string("\xf0\x90\x8c\xbc"));

	// Invalid 4 Octet Sequence (in 2nd Octet) 
	BOOST_REQUIRE(is_invalid_utf8_string("\xf0\x28\x8c\xbc"));

	// Invalid 4 Octet Sequence (in 3rd Octet) 
	BOOST_REQUIRE(is_invalid_utf8_string("\xf0\x90\x28\xbc"));

	// Invalid 4 Octet Sequence (in 4th Octet) 
	BOOST_REQUIRE(is_invalid_utf8_string("\xf0\x28\x8c\x28"));

	// Valid 5 Octet Sequence (but not Unicode!) 
	BOOST_REQUIRE(is_invalid_utf8_string("\xf8\xa1\xa1\xa1\xa1"));

	// Valid 6 Octet Sequence (but not Unicode!) 
	BOOST_REQUIRE(is_invalid_utf8_string("\xfc\xa1\xa1\xa1\xa1\xa1"));
}
