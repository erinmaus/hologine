// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include "core/platform_windows.hpp"
#include "core/threading/thread_local_variable_base.hpp"

holo::thread_local_variable_base::thread_local_variable_base()
{
	slot = TlsAlloc();
	
	if (slot == TLS_OUT_OF_INDEXES)
	{
		initialized = false;
		exception = GetLastError();
	}
	else
	{
		initialized = true;
		exception = 0;
	}
}

holo::thread_local_variable_base::~thread_local_variable_base()
{
	if (initialized)
	{
		TlsFree(slot);
	}
}

void* holo::thread_local_variable_base::get() const
{
	if (initialized)
	{
		return TlsGetValue(slot);
	}
	
	return nullptr;
}

void holo::thread_local_variable_base::set(void* value) const
{
	if (initialized)
	{
		TlsSetValue(slot, value);
	}
}

bool holo::thread_local_variable_base::is_valid() const
{
	return initialized;
}

holo::platform_exception_code holo::thread_local_variable_base::get_platform_exception_code() const
{
	return exception;
}
