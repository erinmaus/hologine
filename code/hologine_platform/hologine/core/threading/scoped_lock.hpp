// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_THREADING_SCOPED_LOCK_HPP_
#define HOLOGINE_CORE_THREADING_SCOPED_LOCK_HPP_

#include "core/threading/mutex.hpp"

namespace holo
{
	class condition_variable_base;

	class scoped_lock final
	{
		friend holo::condition_variable_base;

		public:
			// Locks the provided mutex.
			scoped_lock(holo::mutex& mutex);

			// Unlocks the underlying mutex object.
			~scoped_lock();

		private:
			holo::mutex& mutex;
	};
}

#endif