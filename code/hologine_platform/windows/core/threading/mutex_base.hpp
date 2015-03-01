// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_THREADING_MUTEX_BASE_HPP_
#define HOLOGINE_CORE_THREADING_MUTEX_BASE_HPP_

#include "core/platform_windows.hpp"
#include "core/threading/mutex_interface.hpp"

namespace holo
{
	class condition_variable_base;

	// Windows implementation of a mutex.
	class mutex_base : public mutex_interface
	{
		friend holo::condition_variable_base;

		protected:
			// Implementation.
			bool create_mutex() override;

			// Implementation.
			void destroy_mutex() override;

			// Implementation.
			void lock() override;

			// Implementation.
			void unlock() override;

		private:
			CRITICAL_SECTION critical_section;
	};
}

#endif
