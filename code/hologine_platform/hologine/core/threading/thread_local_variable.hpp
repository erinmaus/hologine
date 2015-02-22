// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_THREADING_THREAD_LOCAL_VARIABLE_HPP_
#define HOLOGINE_CORE_THREADING_THREAD_LOCAL_VARIABLE_HPP_

#include "core/threading/thread_local_variable_base.hpp"

namespace holo
{
	// Defines a platform-agnostic thread local variable.
	//
	// As with holo::thread_local_variable_base, no method will push exceptions.
	template <class Type>
	class thread_local_variable final : public thread_local_variable_base
	{
		thread_local_variable(const thread_local_variable<Type>&) = delete;
		thread_local_variable<Type>& operator =(const thread_local_variable<Type>&) = delete;
		
		public:
			// Attempts to reserves a thread local variable.
			//
			// There is an upper limit to the number of possible thread local
			// variables. If this method fails to reserve a thread local variable,
			// then holo::thread_local_variable_base::get() and
			// holo::thread_local_variable_base::set(void*) will always fail.
			//
			// To query if the holo::thread_local_variable_base was able to reserve a
			// slot, query holo::thread_local_variable_base::is_valid().
			thread_local_variable() = default;
			
			// Releases all resources managed by this instance.
			//
			// This does not free the underlying value; all thread local variables
			// should be disposed of after their values. Doing otherwise will result
			// in a memory leak.
			~thread_local_variable() = default;
			
			// A pointer to the underlying type this thread local variable is storing.
			typedef Type* pointer_type;
			
			// Implicit cast operator to the underlying type.
			//
			// Behaves like a typed holo::thread_local_variable_interface::get().
			operator pointer_type() const
			{
				return (pointer_type)get();
			}
			
			// Assigns the provided value to this thread's slot.
			//
			// Behaves like a typed holo::thread_local_variable_interface::set(void*).
			const thread_local_variable<Type>& operator =(pointer_type value) const
			{
				set(value);
				
				return *this;
			}
	};
}

#endif
