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
#include "core/threading/condition_variable_base.hpp"

bool holo::condition_variable_base::create_condition_variable()
{
	InitializeConditionVariable(&condition_variable);

	return true;
}

void holo::condition_variable_base::destroy_condition_variable()
{
	// Nothing.
}

void holo::condition_variable_base::wait(holo::scoped_lock& lock)
{
	SleepConditionVariableCS(
		&condition_variable,
		&lock.mutex.critical_section,
		INFINITE);
}

void holo::condition_variable_base::notify_one()
{
	WakeConditionVariable(&condition_variable);
}

void holo::condition_variable_base::notify_all()
{
	WakeAllConditionVariable(&condition_variable);
}
