// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_THREADING_MUTEX_HPP_
#define HOLOGINE_CORE_THREADING_MUTEX_HPP_

#include "core/threading/mutex_base.hpp"

namespace holo
{
	class scoped_lock;

	// Represents an exclusive lock on a resource.
	class mutex final : public mutex_base
	{
		friend holo::scoped_lock;
		
		public:
			// Creates the mutex.
			mutex();

			// Releases the resources allocated by the mutex.
			~mutex();
	};
}

#endif
