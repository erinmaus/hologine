// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_THREADING_CONDITION_VARIABLE_INTERFACE_HPP_
#define HOLOGINE_CORE_THREADING_CONDITION_VARIABLE_INTERFACE_HPP_

namespace holo
{
	class scoped_lock;

	// Represents a platform-specific implementation of a condition variable.
	class condition_variable_interface
	{
		protected:
			// Creates the underlying condition variable.
			virtual bool create_condition_variable() = 0;

			// Destroys the underlying condition variable.
			virtual void destroy_condition_variable() = 0;

		public:
			// Waits until the condition variable is signaled.
			virtual void wait(holo::scoped_lock& lock) = 0;

			// Signals a single waiting thread.
			virtual void notify_one() = 0;

			// Signals all waiting threads.
			virtual void notify_all() = 0;
	};
}

#endif
