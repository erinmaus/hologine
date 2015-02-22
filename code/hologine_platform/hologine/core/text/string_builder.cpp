// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include "core/memory/allocator.hpp"
#include "core/text/format.hpp"
#include "core/text/string.hpp"
#include "core/text/string_builder.hpp"

holo::string_builder::string_builder(
	holo::allocator* allocator,
	bool deallocate) :
		buffer_list_head(nullptr),
		buffer_list_tail(nullptr),
		allocator(allocator),
		deallocate_buffer_list(deallocate)
{
	// Nothing...
}

holo::string_builder::~string_builder()
{
	if (deallocate_buffer_list)
	{
		character_buffer* current_buffer = buffer_list_head;

		while (current_buffer != nullptr)
		{
			character_buffer* next = current_buffer->next;

			allocator->deallocate(current_buffer);

			current_buffer = next;
		}
	}
}

holo::string holo::string_builder::to_string(holo::allocator* allocator)
{
	holo::string string;

	// Only allocate a string buffer if the internal string representation is not
	// empty...
	if (length > 0)
	{
		string.buffer = allocator->construct<holo::string::string_buffer>();
		if (string.buffer == nullptr)
		{
			// Out of memory. Let's just hope the caller queries for success.
			return holo::string();
		}

		string.buffer->data = (char*)allocator->allocate(length);
		if (string.buffer->data == nullptr)
		{
			allocator->destruct(string.buffer);

			return holo::string();
		}

		// Finish preparing the string.
		string.storage_mode = holo::string::storage_managed;
		string.buffer->length = length;
		string.buffer->reference_count = 1;

		// Copy the buffers, one at a time, into the string.
		std::size_t string_buffer_index = 0;
		character_buffer* current_character_buffer = buffer_list_head;
		while (current_character_buffer != nullptr)
		{
			std::memcpy(
				string.buffer->data + string_buffer_index,
				get_character_buffer_data(current_character_buffer),
				current_character_buffer->length);

			string_buffer_index += current_character_buffer->length;
			current_character_buffer = current_character_buffer->next;
		}
	}

	return string;
}

bool holo::string_builder::append(const holo::string& string)
{
	const char* string_buffer;
	if (string.storage_mode == holo::string::storage_managed)
	{
		string_buffer = string.buffer->data;
	}
	else
	{
		string_buffer = string.const_string;	
	}

	char* builder_buffer = request_buffer(string.length);
	if (builder_buffer != nullptr)
	{
		std::memcpy(builder_buffer, string_buffer + string.offset, string.length);

		return true;
	}

	return false;
}

bool holo::string_builder::append(const holo::format::format_interface& format_object)
{
	return format_object.append_format(this);
}

char* holo::string_builder::request_buffer(std::size_t length)
{
		character_buffer* buffer = (character_buffer*)allocator->allocate(sizeof(character_buffer) + length);

		if (!buffer)
		{
			return nullptr;
		}

		buffer->next = nullptr;
		buffer->length = length;

		if (buffer_list_tail != nullptr)
		{
			buffer_list_tail->next = buffer;
		}
		else
		{
			buffer_list_head = buffer;
		}

		buffer_list_tail = buffer;

		return get_character_buffer_data(buffer);
}

char* holo::string_builder::get_character_buffer_data(character_buffer* buffer)
{
	return (char*)(buffer + 1);
}
