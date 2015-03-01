// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include "core/threading/scoped_lock.hpp"

holo::scoped_lock::scoped_lock(holo::mutex& mutex) :
	mutex(mutex)
{
	mutex.lock();
}

holo::scoped_lock::~scoped_lock()
{
	mutex.unlock();
}
