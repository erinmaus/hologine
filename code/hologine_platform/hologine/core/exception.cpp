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
#include "core/exception.hpp"
#include "core/memory/allocator.hpp"
#include "core/threading/thread_local_variable.hpp"

// This is arbitrary. It should technically be equal to the number of exception
// codes defined by the program, but that's unknown until startup...
const holo::exception_code holo::max_exception_codes = 256;

holo::exception_code holo::exception_code_generator::current_exception_code = 0;
const char* holo::exception_code_generator::exception_code_enumerations[max_exception_codes] = {};

holo::exception_code holo::exception_code_generator::generate_exception_code(const char* enumeration)
{
	if (!is_initialized())
	{
		initialize();
	}
	
	if (current_exception_code >= max_exception_codes)
	{
		// There's no way to report an error at this point, since we should be
		// executing before main.
		terminate();
	}

	// Set the enumeration, for display or logging errors.
	exception_code_enumerations[current_exception_code] = enumeration;
	
	// Codes are allocated in advance, therefore return the current code and then
	// increment it.
	return current_exception_code++;
}

const char* holo::exception_code_generator::get_exception_enumeration(holo::exception_code code)
{
	if (!is_initialized())
	{
		initialize();
	}

	// 'holo::exception_code' is signed, so we do have to check for less than zero.
	if (code < 0 || code >= current_exception_code)
	{
		push_exception(exception::index_out_of_range);
		
		return nullptr;
	}
	
	return exception_code_enumerations[code];
}

void holo::exception_code_generator::initialize()
{
	// Start at 1; 0 is reserved for holo::exception::none.
	current_exception_code = 1;

	// Map the enumeration.
	exception_code_enumerations[0] = "holo_exception_none";
}

bool holo::exception_code_generator::is_initialized()
{
	return current_exception_code < 1;
}

// holo::exception::none must be 0.
const holo::exception_code holo::exception::none = 0;

// Reserve the generic exceptions.
const holo::exception_code holo::exception::index_out_of_range =
	holo::exception_code_generator::generate_exception_code("holo_exception_index_out_of_range");
const holo::exception_code holo::exception::invalid_operation =
	holo::exception_code_generator::generate_exception_code("holo_exception_invalid_operation");
const holo::exception_code holo::exception::invalid_argument =
	holo::exception_code_generator::generate_exception_code("holo_exception_invalid_argument");
const holo::exception_code holo::exception::platform =
	holo::exception_code_generator::generate_exception_code("holo_exception_platform");

// Implementation of the exception handler.
static holo::thread_local_variable<holo::exception_handler> holo_exception_handler;

bool holo::enable_exceptions(holo::allocator* allocator, holo::exception_code_handler_callback callback)
{
	if (!holo_exception_handler.is_valid())
	{
		// The thread local variable could not allocate a slot.
		// This is actually a pretty horrible state to be in, but it should be
		// handled on the main thread.
		return false;
	}

	holo::exception_handler* handler = allocator->construct<holo::exception_handler>();

	if (handler == nullptr)
	{
		// The allocator failed to allocate a small portion of memory...!
		// This means the allocator is misconfigured or something has exhausted the
		// memory, but most probably the former.
		return false;
	}

	handler->exception_stack_top = 0;
	handler->callback = callback;
	handler->allocator = allocator;

	// Everything (so far) has been successful, so finish by assigning the thread
	// local variable the exception handler.
	holo_exception_handler = handler;

	return true;
}

void holo::disable_exceptions()
{
	// We're in the process of tearing down the exception handler.
	// If we don't unassign the exception handler, bad things can happen if
	// the allocator successfully deallocates the memory but touches the
	// exception handler afterwards, perhaps by pushing or popping an error.
	//
	// So begin by storing a local copy of the exception handler and setting the
	// thread local variable to NULL, which effectively disables the exception
	// handler.
	holo::exception_handler* handler = holo_exception_handler;

	if (handler == nullptr)
	{
		// ...or just return early if the handler is disabled.
		return;
	}

	holo_exception_handler = nullptr;

	// Deallocate the handler... and we're done!
	handler->allocator->destruct(handler);
}

void holo::push_exception(holo::exception_code code, holo::platform_exception_code platform_code)
{
	holo::exception_handler* handler = holo_exception_handler;

	if (handler != nullptr)
	{
		holo_assert(handler->exception_stack_top < exception_handler::max_exception_stack_size);

		handler->callback(code, platform_code);

		exception_handler_node* node = nullptr;
		if (handler->exception_stack_top == handler->exception_stack_top)
		{
			// The stack is full; discard the first element and shift the array.
			exception_handler_node* begin = handler->exception_stack + 1;
			exception_handler_node* end = handler->exception_stack + exception_handler::max_exception_stack_size - 1;

			std::copy(handler->exception_stack, begin, end);

			node = &handler->exception_stack[exception_handler::max_exception_stack_size - 1];
		}
		else
		{
			node = &handler->exception_stack[handler->exception_stack_top++];
		}

		node->exception_code = code;
		node->platform_exception_code = platform_code;

		// Discard the exception code; it's not relevant anymore.
		handler->platform_exception_code = 0;
	}
}

holo::exception_code holo::get_last_exception()
{
	holo::exception_handler* handler = holo_exception_handler;

	if (handler != nullptr)
	{
		holo_assert(handler->exception_stack_top < exception_handler::max_exception_stack_size);

		if (handler->exception_stack_top == 0)
		{
			// The stack is empty, thus there are no errors.
			handler->platform_exception_code = 0;

			return exception::none;
		}
		else
		{
			exception_handler_node* node = &handler->exception_stack[--handler->exception_stack_top];
			handler->platform_exception_code = node->platform_exception_code;

			return node->exception_code;
		}
	}

	// Exception handling is disabled.
	return exception::none;
}

holo::platform_exception_code holo::get_last_platform_exception()
{
	holo::exception_handler* handler = holo_exception_handler;

	if (handler != nullptr)
	{
		return handler->platform_exception_code;
	}

	// Exception handling is disabled.
	return 0;
}

void holo::terminate()
{
	std::terminate();
}
