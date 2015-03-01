// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_THREADING_CONDITION_VARIABLE_HPP_
#define HOLOGINE_CORE_THREADING_CONDITION_VARIABLE_HPP_

#include "core/threading/condition_variable_base.hpp"

namespace holo
{
	// Represents a condition variable.
	class condition_variable final : public condition_variable_base
	{
		public:
			// Creates a condition variable.
			condition_variable();

			// Destroys a condition variable.
			~condition_variable();
	};
}

#endif
