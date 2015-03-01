// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include "core/threading/condition_variable.hpp"

holo::condition_variable::condition_variable()
{
	create_condition_variable();
}

holo::condition_variable::~condition_variable()
{
	destroy_condition_variable();
}
