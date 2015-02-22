// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_THREADING_THREAD_LOCAL_VARIABLE_INTERFACE_HPP_
#define HOLOGINE_CORE_THREADING_THREAD_LOCAL_VARIABLE_INTERFACE_HPP_

namespace holo
{
	class thread_local_variable_interface
	{
			// Gets the current value of the thread local variable.
			//
			// This method returns whatever the current value is; thus, the default
			// value of NULL does not indicate success of failure. Query
			// holo::thread_local_variable::is_valid() to see if this method would
			// succeed.
			virtual void* get() const = 0;
			
			// Sets the current value of the thread local variable.
			//
			// This method does nothing if the underlying thread local variable is
			// invalid.
			virtual void set(void* value) const = 0;
			
			// Gets if the current thread local variable is in a valid state.
			//
			// If the variable could not be allocated, then this returns false.
			// Query holo::thread_local_variable_base::get_platform_exception_code()
			// for the platform-specific reason.
			virtual bool is_valid() const = 0;
			
			// Gets the platform exception.
			//
			// There are no guarantees as to the nature of this value. To query if
			// the thread local variable is valid, use
			// holo::thraed_local_variable_base::is_valid() instead. If the
			// aforementioned method returns false, then the return value of this
			// method will be a valid platform exception code.
			virtual holo::platform_exception_code get_platform_exception_code() const = 0;
	};
}

#endif
