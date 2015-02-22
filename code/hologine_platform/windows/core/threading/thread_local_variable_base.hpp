// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_THREADING_THREAD_LOCAL_VARIABLE_BASE_HPP_
#define HOLOGINE_CORE_THREADING_THREAD_LOCAL_VARIABLE_BASE_HPP_

#include "core/exception.hpp"
#include "core/platform_windows.hpp"
#include "core/threading/thread_local_variable_interface.hpp"

namespace holo
{
	// Defines the platform-specific internals of a thread local variable.
	//
	// No method will push an exception. Instead, query success with
	// holo::thread_local_variable_base::is_valid() and
	// holo::thread_local_variable_base::get_platform_exception_code().
	class thread_local_variable_base : public thread_local_variable_interface
	{
		public:
			// See holo::thread_local_variable::thread_local_variable() for
			// documentation and expected behavior.
			thread_local_variable_base();
			
			// See holo::thread_local_variable::~thread_local_variable() for
			// documentation and expected behavior.
			virtual ~thread_local_variable_base();
			
			// Implementation.
			void* get() const override;
			
			// Implementation.
			void set(void* value) const override;
			
			// Implementation.
			bool is_valid() const override;
			
			// Implementation.
			holo::platform_exception_code get_platform_exception_code() const override;
		
		private:
			// Whether or not the underlying slot was successfully initialized, and
			// thus, whether or not the thread local variable is valid.
			bool initialized;
			
			// The platform exception code, if any.
			holo::platform_exception_code exception;
			
			// The slot where this variable is located.
			DWORD slot;
	};
}

#endif
