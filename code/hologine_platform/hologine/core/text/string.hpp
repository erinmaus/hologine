// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations, most
// notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_TEXT_STRING_HPP_
#define HOLOGINE_CORE_TEXT_STRING_HPP_
#include <iterator>
#include "core/memory/allocator.hpp"
#include "core/text/string_iterator.hpp"
#include "core/text/unicode.hpp"

namespace holo
{
	class allocator;
	class string_builder;

	// A small, relatively immutable UTF-8 string class.
	//
	// Although most of the C++ Standard Library is retrofitted to work with
	// Hologine (e.g., via the custom std::allocator proxy, aliases for different
	// containers) if the container is fit for the job, the built-in string is
	// simply ill fit for any purpose:
	//
	// 1) It's not immutable. A string may be altered, which results in...
	// 2) Inefficient substrings. std::string::substr() allocates another char
	//    array for every substring created... Most string implementations have a
	//    small buffer to store small strings for this reason! That's clearly a
	//    performance hack, not a good design decision.
	// 3) Doesn't natively support UTF-8 or Unicode. Although this can be fixed
	//    with custom iterators, it means a string is nothing more than a dynamic
	//    byte array (because it's not immutable; see 1).
	//
	// holo::string is immutable by interface; this means that the underlying
	// buffer cannot change. To change a string, a new buffer must be created. A
	// string object itself manages the underlying buffer, providing immutable
	// access by design.
	//
	// String objects should not be shared obliviously between threads, even if
	// the provided allocator is thread-safe, because of internal bookkeeping
	// performed between string object instances.
	//
	// The best method of sharing strings between threads would be to duplicate it
	// using a thread safe allocator once per needed thread. This unlinks it from
	// any prior string object instances and ensures thread safety.
	//
	// The string object only allocates POD types.
	class string
	{
		friend class holo::string_iterator;
		friend class holo::string_builder;
		friend bool operator ==(const holo::string& left, const holo::string& right);
		friend bool operator !=(const holo::string& left, const holo::string& right);

		public:
			// Aliases an object that allows iterating over individual 'code points'
			// in the string.
			//
			// The underlying string is stored as UTF-8. Invalid character sequences
			// or code points will result push errors; iterating further is undefined.
			// For more information, refer to holo::string_iterator.
			//
			// The elements of a string cannot be modified, thus a const_iterator is
			// the only iterator defined.
			typedef string_iterator const_iterator;

			// Creates an empty string.
			string();

			// Creates a string from a C-style string.
			//
			// Since no allocator is provided, the underlying character data is
			// considered unmanaged and must have a longer lifetime than the string
			// instance.
			//
			// The provided string should be valid ASCII or UTF-8.
			string(const char* data);

			// Creates a string from a C-style string.
			//
			// The provided allocator will be used to allocate memory for the
			// underlying buffer, since a copy will be made of the provided string.
			//
			// The provided string should be valid ASCII or UTF-8.
			string(holo::allocator* allocator, const char* other);

			// Creates a string from a range, including 'first' but excluding 'last'.
			//
			// The provided allocator will be used to allocate memory for the
			// underlying buffer.
			//
			// If the provided iterator permits random access, this operation will
			// perform faster in comparison to if the iterator does not permit random
			// access.
			//
			// Regardless of the iterator type, the iterator should return raw
			// character data when dereferenced.
			//
			// In the case of a random access iterator, the required length can be
			// calculated by distance of 'first' and 'last', and then a linear copy
			// operation will be performed.
			//
			// Otherwise, two linear options will be performed: one to calculate the
			// length of the string, and one to copy the string into the internal
			// buffer.
			//
			// The provided string should be valid ASCII or UTF-8.
			template <class ForwardIterator>
			string(holo::allocator* allocator, ForwardIterator first, ForwardIterator last);

			// Copy constructor.
			string(const holo::string& other);

			// Releases all unshared resources reserved by the string object.
			~string();

			// Returns a character iterator.
			//
			// The iterator, when dereferenced, will return a Unicode code point; for
			// more information on the behavior of a holo::string_iterator, refer to
			// the class declaration.
			string_iterator begin() const;

			// Returns a character iterator to the end of a string.
			string_iterator end() const;

			// Creates and returns a substring from iterators pointing to codepoints
			// within the string instance, including 'first' but excluding 'last'.
			//
			// The iterators must belong to this string instance, otherwise
			// holo::exception::invalid_argument is pushed.
			holo::string substring(string_iterator first, string_iterator last) const;

			// Duplicates the string.
			//
			// The returned string will use the same allocator, but be unique from
			// the original string.
			//
			// An extended example to duplicate a substring would thus be:
			//
			// string original = "foobar";
			// string substring = original.substring(original.at(3), original.end());
			// string duplicate = substring.duplicate();
			//
			// Returns a duplicate string on success; on failure, returns an empty
			// string. Failure only occurs if the underlying allocator fails, thus
			// the top-most exception will be the allocator's exception.
			holo::string duplicate() const;

			// Duplicates the string.
			//
			// The returend string will use the provided allocator. Behaves otherwise
			// like holo::string::duplicate().
			holo::string duplicate(holo::allocator* allocator) const;

			// Assigns the string the data of another.
			//
			// The string this instance represents will thus be equivalent to
			// 'other'.
			//
			// This method cannot fail, unless the computer is unplugged, the RAM is
			// bad, or a power surge reprograms the instruction cache.
			holo::string& operator =(const holo::string& other);

		private:
			struct string_buffer
			{
				// The buffer of raw UTF-8 sequences that compose the string.
				//
				// This buffer should not be NULL-terminated.
				char* data;

				// The length of the buffer, in bytes.
				std::size_t length;

				// A reference count to this buffer object.
				//
				// A string buffer object starts life with a reference count of 1. Every
				// string object that refers to this buffer increments the reference
				// count by 1. When the string object no longer uses a buffer, the
				// buffer's reference count is decremented by 1. Upon the buffer
				// object's reference count reaching 0, the buffer object is destroyed.
				std::size_t reference_count;

				// Allocator used to allocate the string buffer object and the
				// underlying string data.
				holo::allocator* allocator;
			};

			// Storage method of the internal string.
			enum
			{
				// The string object manages storage of the internal character data.
				//
				// This is the default behavior, unless a raw C-style string is wrapped
				// by the object.
				storage_managed,

				// The string object does not manage the character data.
				//
				// This is the behavior when given a raw C-style string and no
				// allocator.
				storage_unmanaged
			};

			// Storage mode of internal character data.
			int storage_mode;

			// Anonymous union, supportng the ability to store raw C-style strings
			// without having to allocate memory.
			//
			// Which member is used depends on 'storage_mode'. A managed string will
			// use 'buffer', while a raw C-style string will use 'const_string'.
			union
			{
				string_buffer* buffer;
				const char* const_string;
			};

			// References a string_buffer, incrementing its reference count by 1.
			//
			// If 'buffer' is NULL, this method does nothing.
			static void reference_buffer(string_buffer* buffer);

			// Dereferences a string_buffer, decrementing its reference count by 1.
			//
			// If the string_buffer becomes unused, then it will be destructed and
			// all resources will be released.
			//
			// If 'buffer' is NULL, this method does nothing.
			static void dereference_buffer(string_buffer* buffer);

			// Constructs a string buffer from a range.
			//
			// On success a valid string buffer object; on failure, returns NULL.
			// Failure indicates the allocator could not provide the memory for the
			// allocation of the string buffer object.
			template <class ForwardIterator>
			static string_buffer* construct_buffer(
				holo::allocator* allocator,
				ForwardIterator first,
				ForwardIterator last);

			// Index into the beginning of the character data, in bytes.
			//
			// Used by substrings to reference a smaller portion of a larger string.
			std::size_t offset;

			// Length of the string, in bytes.
			//
			// Like 'offset', this value can be used to reference a smaller portion of
			// a larger string.
			std::size_t length;
	};

	template <class ForwardIterator>
	string::string(
		holo::allocator* allocator,
		ForwardIterator first,
		ForwardIterator last) :
			offset(0),
			length(0)
	{
		buffer = construct_buffer(allocator, first, last);

		// An empty string has no buffer, thus if the allocation fails, the string
		// will be in a valid, but unexpected, state.
		if (buffer != nullptr)
		{
			length = buffer->length;
		}
	}

	template <class ForwardIterator>
	holo::string::string_buffer* string::construct_buffer(
		holo::allocator* allocator,
		ForwardIterator first,
		ForwardIterator last)
	{
		string_buffer* buffer = allocator->construct<string_buffer>();
		if (buffer != nullptr)
		{
			// For RandomAccessIterators, this will be O(1); for any other, it will be
			// O(N). Regardless, the distance is calculated first so that only one
			// allocation of the data buffer must be performed.
			buffer->length = std::distance(first, last);

			// Allocate the buffer. If this fails, bail out.
			//
			// The buffer is not NULL-terminated, and the input iterator is expected
			// to dereference as a char; thus, simply allocate enough memory for
			// 'length' bytes.
			//
			// Since we're dealing with raw byte data, signify padding is unnecessary.
			buffer->data = (char*)allocator->allocate(buffer->length, 1);

			if (buffer->data != nullptr)
			{
				// Finish preparing the buffer object.
				buffer->reference_count = 1;
				buffer->allocator = allocator;

				// Copy the data. The iterator is expected to return char data.
				std::size_t index = 0;
				while (first != last)
				{
					buffer->data[index++] = *first++;
				}
			}
			else
			{
				// Deallocate and consider the string empty.
				allocator->destruct(buffer);
				buffer = nullptr;
			}
		}

		return buffer;
	}

	// Compares two strings for lexical equivalence.
	bool operator ==(const holo::string& left, const holo::string& right);

	// Compares two strings to lexical inequivalence.
	bool operator !=(const holo::string& left, const holo::string& right);
}

#endif
