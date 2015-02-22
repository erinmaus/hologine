// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include <process.h>
#include "core/exception.hpp"
#include "core/platform_windows.hpp"
#include "core/threading/thread.hpp"

bool holo::thread_base::create_thread(thread_argument* argument)
{
	// In order to prevent the MSCRT from leaking memory, we must use _beginthreadex.
	// The problem arises when a thread calls a function in the C standard library;
	// the initialization will be performed, but deinitialiation will not occur
	// on thread exit...
	thread_handle =
		(HANDLE)_beginthreadex(nullptr, 0, &win32_thread_proc, argument, CREATE_SUSPENDED, nullptr);
	
	if (thread_handle == nullptr)
	{
		push_exception(exception::platform, GetLastError());
		
		return false;
	}
	
	return true;
}

bool holo::thread_base::run_thread()
{
	if (ResumeThread(thread_handle) == (DWORD)-1)
	{
		push_exception(exception::platform, GetLastError());
		
		return false;
	}
	
	return true;
}

bool holo::thread_base::join_thread()
{
	DWORD result = WaitForSingleObject(thread_handle, INFINITE);
	
	// WAIT_TIMEOUT shouldn't happen unless the Universe expired and
	// WAIT_ABANDONED is exclusively for mutexes, so...
	if (result == WAIT_FAILED)
	{
		push_exception(exception::platform, GetLastError());
		return false;
	}
	
	CloseHandle(thread_handle);
	thread_handle = nullptr;
	
	return true;
}

unsigned int WINAPI holo::thread_base::win32_thread_proc(void* parameter)
{
	thread_argument* argument = (thread_argument*)parameter;
	bool exceptions_enabled = false;
	
	if ((argument->flags & flag_enable_exceptions) && argument->allocator != nullptr)
	{
		// If the exception handler fails to be created, then don't bother disabling
		// exceptions; the exception handler will be in a clean state on failure.
		exceptions_enabled = enable_exceptions(argument->allocator, nullptr);
	}
	
	argument->return_status = argument->callback(argument->userdata);
	
	// Only disable exceptions if they were enabled.
	if (exceptions_enabled)
	{
		disable_exceptions();
	}
	
	return 0;
}
