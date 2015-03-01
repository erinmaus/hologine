// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_THREADING_MUTEX_INTERFACE_HPP_
#define HOLOGINE_CORE_THREADING_MUTEX_INTERFACE_HPP_

namespace holo
{
	// Represents a platform-specific implementation of a mutex.
	class mutex_interface
	{
		protected:
			// Creates the mutex, returning true on success, false otherwise.
			virtual bool create_mutex() = 0;

			// Destroys the previously created mutex.
			virtual void destroy_mutex() = 0;

			// Locks the mutex.
			virtual void lock() = 0;

			// Unlocks the mutex.
			virtual void unlock() = 0;
	};
}

#endif
