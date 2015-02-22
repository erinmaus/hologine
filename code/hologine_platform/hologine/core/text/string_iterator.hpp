// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_TEXT_STRING_ITERATOR_HPP_
#define HOLOGINE_CORE_TEXT_STRING_ITERATOR_HPP_

#include <boost/iterator/iterator_facade.hpp>
#include "core/text/unicode.hpp"

namespace holo
{
	class string;

	class string_iterator :
		public boost::iterator_facade<
			string_iterator,
			holo::unicode::code_point,
			boost::bidirectional_traversal_tag,
			holo::unicode::code_point>
	{
		friend class holo::string;
		friend class boost::iterator_core_access;

		public:
			// Creates an empty string iterator.
			//
			// This iterator will match no iterator other than itself.
			string_iterator();

			// Gets a value indicating if the current code point is valid.
			//
			// Returns true if the current code point is valid, false otherwise.
			//
			// Iterating beyond an invalid character is not possible; attempts to do
			// so will terminate the program. Therefore, validate input!
			bool get_is_valid() const;

		private:
			// Creates a string iterator from the provided string.
			//
			// 'offset' should be aligned on a UTF-8 sequence.
			string_iterator(const holo::string* string, std::size_t offset = 0);

			// Implementation.
			holo::unicode::code_point dereference() const;

			// Implementation.
			void increment();

			// Implementation.
			void decrement();

			// Implementation.
			bool equal(const holo::string_iterator& other) const;

			// The string this iterator is iterating over.
			const holo::string* string;

			// The next offset, relative to the string's offset.
			//
			// This value points to the end of the current sequence (and thus to
			// the beginning of the next sequence, if any).
			std::size_t next_offset;

			// Current offset, relative to the string's offset.
			//
			// This value points to the starting position of where
			// 'cached_code_point_value' was decoded. Therefore, this value should be
			// used to split a string, for example; if 'current_offset' is used, the
			// first code point will be unexpectedly missing.
			std::size_t current_offset;

			// This is a cached value from incrementing the code point.
			holo::unicode::code_point cached_code_point_value;

			// A boolean indicating if this iterator is valid.
			bool is_valid;
	};
}

#endif
