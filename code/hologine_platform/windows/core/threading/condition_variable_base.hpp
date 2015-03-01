// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_THREADING_CONDITION_VARIABLE_BASE_HPP_
#define HOLOGINE_CORE_THREADING_CONDITION_VARIABLE_BASE_HPP_

#include "core/platform_windows.hpp"
#include "core/threading/condition_variable_interface.hpp"

namespace holo
{
	// Windows implementation of a condition variable.
	class condition_variable_base : public condition_variable_interface
	{
		protected:
			// Creates the underlying condition variable.
			bool create_condition_variable() override;

			// Destroys the underlying condition variable.
			void destroy_condition_variable() override;

		public:
			// Implementation.
			void wait(holo::scoped_lock& lock) override;

			// Implementation.
			void notify_one() override;

			// Implementation.
			void notify_all() override;

		private:
			CONDITION_VARIABLE condition_variable;
	};
}

#endif
