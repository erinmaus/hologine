// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_THREADING_THREAD_INTERFACE_HPP_
#define HOLOGINE_CORE_THREADING_THREAD_INTERFACE_HPP_

#include "core/memory/allocator.hpp"

namespace holo
{
	// Return value of a thread.
	//
	// If necessary, a thread can return a small status code upon exit.
	//
	// Generally, a more robust method of error handling should be used. Of
	// course, if the thread is simple enough (e.g., it does not enable the
	// exception code handler), and can only fail in a few precise places,
	// then using a status code would be preferable.
	//
	// The value of 0 is reserved by thread_return_status_ok, and is the value
	// that should be returned if the thread was successful.
	typedef unsigned char thread_return_status;
	
	// Indicates the thread successfully finished its task.
	extern const thread_return_status thread_return_status_ok;
	
	// Signature of a thread callback.
	//
	// The 'userdata' parameter is used to pass initial data to the thread. The
	// actual value represented would be determined by the thread callback.
	//
	// The thread should return a value upon exit (see holo::thread_return_status).
	typedef thread_return_status (* thread_callback)(void* userdata);
	
	// Represents the platform-specific implementation of a memory region and
	// exposes the necessary methods.
	class thread_interface
	{
		public:
			// Flags that indicate or modify the behavior of the thread.
			enum
			{
				// Whether or not exceptions are enabled at startup.
				flag_enable_exceptions = 0x00000001,
				
				// Whether or not the thread was successfully created.
				flag_thread_created = 0x00000002,
				
				// Whether or not the thread failed to be created.
				//
				// If thread creation failed, further operations should generate the
				// appropriate error.
				flag_thread_invalid = 0x00000004,
				
				// Whether or not the thread is currently running, or has previously
				// been run.
				//
				// This flag should be set even if the thread exits.
				flag_thread_started = 0x00000008,
				
				// Whether or not the thread has exited.
				//
				// If this flag is enabled, then all resources should have been released
				// to the system (i.e., after a successful call to
				// holo::thread_interface::join_thread()).
				flag_thread_exited = 0x00000010
			};
			
			// Argument passed to the internal thread callback.
			struct thread_argument
			{
				// The return status of the thread callback.
				//
				// This value should be set when the thread callback returns.
				holo::thread_return_status return_status;
				
				// The thread callback.
				//
				// This is the method that should be called after initializing the
				// platform-specific thread.
				holo::thread_callback callback;
				
				// The userdata to pass to the callback.
				//
				// This value is optional and defaults to NULL.
				void* userdata;
				
				// Flags that modify the behavior of the thread.
				//
				// See the enumeration above for a list of flags.
				int flags;
				
				// The default allocator for the thread.
				holo::allocator* allocator;
			};

			// Creates a thread, but does not yet start it.
			//
			// Returns true on success, false on failure. This method should push
			// the appropriate error on failure.
			virtual bool create_thread(thread_argument* argument) = 0;

			// Runs the previously created thread.
			//
			// Returns true on success, false on failure. This method should push
			// the appropriate error on failure.
			virtual bool run_thread() = 0;

			// Joins with the previously created thread.
			//
			// If successful, this method should dispose of the resources allocated
			// for the thread.
			//
			// Returns true on success, false on failure. This method should push
			// the appropriate error on failure.
			virtual bool join_thread() = 0;
	};
}

#endif
