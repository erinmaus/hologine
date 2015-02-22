// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations, most
// notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_TEXT_STRING_BUILDER_HPP_
#define HOLOGINE_CORE_TEXT_STRING_BUILDER_HPP_

#include <cstdlib>

namespace holo
{
	class allocator;
	class string;

	namespace format
	{
		class format_interface;
	}

	// Builds a single holo::string from a collection of smaller strings
	// efficiently.
	//
	// holo::string_builder provides an object-based interface for building large
	// strings from smaller strings. The string builder can achieve efficiency by
	// limiting memory allocations to a certain pattern.
	//
	// Providing the string builder a proper allocator is crucial to maximize
	// efficiency. Only POD types are allocated. By tuning the allocation
	// parameters, a string_builder can be made to work nicely with various
	// allocators.
	//
	// Custom data formatting is possible via custom format objects. These are
	// used to format basic types, such as floats and integers, but also custom
	// types that implement custom string formatters.
	//
	// All data formatting is done via custom methods; although the standard C
	// printf-family of functions for strings are nice, snprintf is not as
	// portable as it should be--and most importantly, it is not as efficient to
	// use as it could be if implemented in a different manner.
	//
	// The reason is that, to determine the size of a string that needs to be
	// printed, snprintf must be called twice. This means that the format
	// specifier must also be parsed twice (and all arguments must be accounted
	// for twice, as well). So whatever gains could be used by using snprintf are
	// similary lost when the underlying size of the object is unknown. Sadly,
	// this is the case in most instances.
	class string_builder
	{
		public:
			// Creates a string builder using the provided allocator.
			//
			// The optional 'deallocate' flag indicates if the string builder should
			// deallocate its memory on destruction. If false, then no memory will be
			// manually deallocated. This is useful if the builder is provided a
			// temporary allocator, or something like a stack allocator (see below
			// example).
			//
			// The provided allocator does not have to support destruction of
			// objects. The holo::string_builder instance guarantees that all types
			// it allocates are POD.
			//
			// An effective way of using the string builder would be to use a stack
			// allocator. Push a marker on to the stack allocator, build the string,
			// call holo::string_builder::to_string(holo::allocator*), and then pop
			// the marker from the stack. In such a case, 'deallocate' can be set to
			// false.
			string_builder(
				holo::allocator* allocator,
				bool deallocate = true);

			// Destructs the string builder, freeings resources if necessary.
			~string_builder();

			// Appends a string to the builder.
			//
			// Returns true on success, false on failure. Failure can occur if the
			// provided allocator could not fulfill an allocation. A relevant
			// exception will be propagated, regardless.
			bool append(const holo::string& string);

			// Appends a formatted object to the builder.
			//
			// Returns true on success, false on failure. Failure can occur if the
			// provided allocator could not fulfill an allocation. A relevant
			// exception will be propagated, regardless.
			bool append(const holo::format::format_interface& format_object);

			// Converts the string builder to a string.
			//
			// The string will use the provided allocator. This should be different
			// from the string builder if a temporary allocator was used and the
			// string is expected to have a longer duration. I.e., if a stack
			// allocator was used to allocate internal holo::string_builder data,
			// and the string is more-or-less longer lived than the string builder,
			// then the string should use a general purpose allocator.
			// Since string builders should probably be used with simple, POD
			// allocators, since their lifetime is short, and a holo::string object
			// should be allocated with a more general allocator, this is the reason
			// an explicit allocator for the final string itself is not optional.
			//
			// Returns an equivalent string on success. On failure, returns an empty
			// string. Failure can occur when the provided allocator is exhausted or
			// could otherwise not fullfill the request. A relevant exception will be
			// propogated in such a case.
			//
			// Of course, if the holo::string_builder instance was used to build an
			// empty string...
			holo::string to_string(holo::allocator* allocator);

			// Requests a buffer, 'size' bytes large.
			//
			// Generally, this method is exposed so holo::format
			//
			// The entire buffer should be filled. Therefore, calling methods
			// should know the size of data necessary in advance.
			//
			// Returns the buffer on success; otherwise, returns NULL. An appropriate
			// error will be pushed.
			char* request_buffer(std::size_t length);

		private:
			// Intrusive character buffer node.
			//
			// The string data is stored after the buffer.
			struct character_buffer
			{
				// The next character buffer in the node.
				character_buffer* next;

				// Size of 'data', in bytes.
				std::size_t length;
			};

			// Gets the data managed by the provided buffer.
			char* get_character_buffer_data(character_buffer* buffer);

			// List of character buffers.
			character_buffer* buffer_list_head;

			// Tail of the character buffers.
			character_buffer* buffer_list_tail;

			// Accumulated length of text data in the buffers.
			std::size_t length;

			// The allocator used by this string builder instance.
			holo::allocator* allocator;

			// Whether or not to deallocate the buffers stored in 'buffer_list' on
			// destruction of the holo::string_builder.
			//
			// This can be a large optimization in certain cases (large string
			// builder instances, or lots of small-to-medium string builders) when a
			// specialized allocator that supports O(1) deallocate of ranges is used
			// (i.e., stack allocator).
			bool deallocate_buffer_list;
	};
}

#endif
