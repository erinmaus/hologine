// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_THREADING_THREAD_BASE_HPP_
#define HOLOGINE_CORE_THREADING_THREAD_BASE_HPP_

#include "core/platform_windows.hpp"
#include "core/threading/thread_interface.hpp"

namespace holo
{
	// Windows implementation of a thread.
	class thread_base : public thread_interface
	{
		public:
			// Implementation.
			bool create_thread(thread_argument* argument) override;
			
			// Implementation.
			bool run_thread() override;
			
			// Implementation.
			bool join_thread() override;
			
		private:
			// The platform-specific thread callback.
			static unsigned int WINAPI win32_thread_proc(void* parameter);
			
			HANDLE thread_handle;
	};
}

#endif
