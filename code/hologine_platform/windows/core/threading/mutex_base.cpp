// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include "core/threading/mutex_base.hpp"

bool holo::mutex_base::create_mutex()
{
	InitializeCriticalSection(&critical_section);

	return true;
}

void holo::mutex_base::destroy_mutex()
{
	DeleteCriticalSection(&critical_section);
}

void holo::mutex_base::lock()
{
	EnterCriticalSection(&critical_section);
}

void holo::mutex_base::unlock()
{
	LeaveCriticalSection(&critical_section);
}
