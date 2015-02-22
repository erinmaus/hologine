// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_EXCEPTION_HPP_
#define HOLOGINE_CORE_EXCEPTION_HPP_

#include <cstddef>
#include <cassert>
#include "core/platform.hpp"

// Triggers a breakpoint on a critical logic error.
#define holo_assert assert

namespace holo
{
	class allocator;
	
	// Represents an exception code.
	typedef int exception_code;
	
	// Represents a platform-specific exception code.
	typedef intptr_t platform_exception_code;
	
	// Constant for the maximum number of exception codes.
	//
	// Any attempts to allocate an exception beyond this range is an error.
	// holo::exception_code_generator::generate_exception_code(const char*)
	// will return holo::exception::none.
	extern const int max_exception_codes;
	
	// Generates codes for exceptions at startup.
	//
	// Rather than define all exceptions in one place and at one time, the
	// holo::exception_code_generator allows lazy creation of error codes.
	//
	// However, this means exceptions may not have the same code between sessions
	// or builds; therefore, an exception_handle should have a brief enumeration
	// stored as a string to log or otherwise display the error.
	//
	// Static members of this class are not thread-safe. Codes should only be
	// created on one thread and only at startup by a holo::exception_handle.
	class exception_code_generator
	{
		exception_code_generator() = delete;
		exception_code_generator(const exception_code_generator&) = delete;
		exception_code_generator& operator =(const exception_code_generator&) = delete;
		
		public:
			// Allocates and returns an exception code.
			//
			// This exception code will be a positive, non-zero number, unique from
			// previous calls.
			//
			// A properly formatted enumeration is in the form
			// "holo_exception_[name]", where 'name' is the identifier associated
			// with the exception in code. For example, holo::exception::none is
			// holo_exception_none while holo::exception::platform is
			// holo_exception_platform. Simple!
			//
			// This method will terminate the process if the code could not be
			// generated (i.e., the maximum number of codes has been reached).
			static holo::exception_code generate_exception_code(const char* enumeration);
			
			// Returns the enumeration of an exception code.
			//
			// If the provided exception_code is invalid, this method pushes
			// holo::exception::index_out_of_range and returns NULL.
			static const char* get_exception_enumeration(holo::exception_code code);
		
		private:
			// Initializes the exception code generator.
			//
			// This allows the exception code generator to be initialized lazily.
			static void initialize();
			
			// Gets if the exception code generator is initialized.
			//
			// Upon startup, the exception code generator has yet to allocate the
			// first exception code.
			static bool is_initialized();
			
			// Storage for defined enumerations.
			static const char* exception_code_enumerations[];
			
			// Current exception code. A value of 0 (holo::exception::none) is
			// reserved to mean no exception.
			static holo::exception_code current_exception_code;
	};
	
	// Namespace for all exceptions.
	//
	// Any exception should be shoved in this namespace.
	namespace exception
	{
		// Represents no exception has occurred.
		//
		// This is also the return value of holo::get_last_exception() if exception
		// handling is not enabled on the current thread.
		extern const exception_code none;
		
		// Represents an error where the provided index is out of the acceptable
		// range.
		//
		// For example, if an index is passed to a method that fetches from an
		// error, and the index is larger than the size of the array, the method
		// should push this error.
		extern const exception_code index_out_of_range;
		
		// Represents an error where the operation is invalid for the current state
		// of the object.
		//
		// For example, starting a thread without supply a thread callback would
		// generate this error.
		extern const exception_code invalid_operation;
		
		// Represents an error where the provided argument to a method was invalid.
		//
		// For example, passing NULL to a method that expects non-NULL may generate
		// this error.
		extern const exception_code invalid_argument;
		
		// Represents an error from the underlying platform that cannot be
		// translated effectively into a platform-agnostic error code.
		//
		// Generally, platform exceptions should provide the platform-specific
		// error code.
		extern const exception_code platform;
	}

	// A method that is called when an exception is pushed on to the stack.
	//
	// This method can be used to log information about the exception, such as
	// the program counter or call stack.
	typedef void(*exception_code_handler_callback)(
		holo::exception_code code, holo::platform_exception_code platform_code);

	// Internal structure for the exception handler, representing data associated
	// with an exception.
	struct exception_handler_node
	{
		// The exception that occurred.
		holo::exception_code exception_code;

		// The platform exception code. This platform depends on the source of the
		// error; it could be a Glenum from glGetError, or a Windows error code
		// from GetLastError, or a possible errno value, or so on. It also may not
		// be provided by the caller. Therefore, this value should, at best, be
		// only used as a hint while debugging.
		holo::platform_exception_code platform_exception_code;
	};

	// Internal structure representing data used by the exception handler.
	struct exception_handler
	{
		// The maximum size of the error stack. If the exception count reaches this
		// value, the oldest exception will be discarded and the new exception
		// will be pushed on to the top of the stack.
		enum { max_exception_stack_size = 32 };

		// The exception stack.
		exception_handler_node exception_stack[max_exception_stack_size];

		// Index of the next top level element in the stack.
		//
		// If this value is 0, the stack is empty. If this value is 1, there is one
		// entry in the stack, and etc. A value equal to max_exception_stack_size
		// indicates the stack is exhausted.
		std::size_t exception_stack_top;

		// The holo::platform_exception_code from the last call to
		// holo::get_last_exception(). This value is retrieved by calling
		// holo::get_last_platform_exception().
		holo::platform_exception_code platform_exception_code;

		// The exception code handler, or NULL if one was not provided.
		holo::exception_code_handler_callback callback;

		// The allocator used to allocate the exception handler. Used to deallocate
		// the exception handler when holo::disable_exceptions() is called.
		holo::allocator* allocator;
	};
	
	// Enables exceptions on the current thread.
	//
	// Exceptions must be enabled on each thread that wishes to retrieve them.
	// For example, a resource loading thread will obviously want exceptions,
	// so it knows why a resource was not loaded. However, a thread that simply
	// does number crunching will generally not need to check for exceptions.
	//
	// The provided allocator will be used to allocate the exception stack.
	//
	// If the holo::exception_code_handler_callback is provided, then this method
	// will be called for every invocation of holo::push_exception(holo::exception_code).
	//
	// The callback can be changed after enabling exceptions by calling this method
	// again. As long as holo::disable_exceptions() was not called, then no
	// exceptions will be lost; only the callback changed.
	//
	// Returns true if the exception stack could be allocated, false otherwise.
	bool enable_exceptions(
		holo::allocator* allocator,
		holo::exception_code_handler_callback callback);
	
	// Disables exceptions on the current thread.
	//
	// Any exceptions on the stack will be discarded. To re-enable exceptions,
	// simply call
	// holo::enable_exceptions(holo::allocator*, holo::exception_code_handler_callback).
	void disable_exceptions();
	
	// Pushes an exception on to the exception stack for the current thread.
	//
	// The optional 'platform_code' parameter is used to indicate a more
	// specific error code returned by the system. For example, this could be
	// the return value of GetLastError, or the value of errno, or the return
	// value of some method.
	void push_exception(
		holo::exception_code code, holo::platform_exception_code platform_code = 0);
	
	// Pops an exception from the exception stack for the current thread.
	//
	// The most recent error is at the top of the stack, while the least recent
	// error is at the bottom.
	//
	// If the exception stack is empty, this returns holo::exception::none. To
	// iterate over all errors from a call, the logic would be something like:
	//
	//   holo::exception_code code
	//   while((code = holo::get_last_exception()) != holo::exception::none)
	//     process_exception(code); // or otherwise handle the exception
	//
	// This method returns holo::exception::none if exception handling is
	// disabled on the current thread.
	holo::exception_code get_last_exception();
	
	// Gets the holo::platform_exception_code of the last exception popped from
	// the stack.
	//
	// If the exception handler is modified, either by pushing a new exception
	// onto the stack or retrieving the latest exception, this value will be
	// discarded (on push) or updated (on pop).
	holo::platform_exception_code get_last_platform_exception();

	// Terminates the program on a fatal error.
	void terminate();
}

#endif
