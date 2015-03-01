// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include "core/threading/mutex.hpp"

holo::mutex::mutex()
{
	create_mutex();
}

holo::mutex::~mutex()
{
	destroy_mutex();
}
