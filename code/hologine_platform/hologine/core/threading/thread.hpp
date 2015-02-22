// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_THREADING_THREAD_HPP_
#define HOLOGINE_CORE_THREADING_THREAD_HPP_

#include "core/memory/allocator.hpp"
#include "core/threading/thread_base.hpp"

namespace holo
{
	// Creates and manages a single thread directly from the underlying platform.
	class thread final : private thread_base
	{
		thread(const holo::thread&) = delete;
		holo::thread& operator =(const holo::thread&) = delete;
		
		public:
			// Creates an empty thread.
			//
			// To provide a thread callback, call
			// holo::thread::start(holo::thread_callback). This will immediately
			// begin executing the thread, however, unlike
			// holo::thread::thread(holo::thread_callback).
			thread();
			
			// Creates a new thread using the provided callback.
			//
			// The thread is created in a waiting state. To start the thread, call
			// holo::thread::start(). When the thread is finished executing,
			// call holo::thread::join() to retrieve its return value.
			//
			// When the holo::thread object is destructed, the destructor will
			// call holo::thread::join(). If the thread is not signalled to end,
			// this may cause a deadlock. Ensure that the thread is winding down or
			// has already exited.
			//
			// If the thread could not be created, an appropriate error will be
			// pushed. Query holo::get_last_exception() after construction.
			thread(holo::thread_callback callback, void* userdata);
			
			// Releases all resources used by this thread, joining with the thread
			// if it has not yet been done.
			~thread();
			
			// Begins executing a thread.
			//
			// If the thread has not been assigned a callback, then this method will
			// push holo::exception::invalid_operation. To start a thread where a
			// callback was not provided, call
			// holo::thread::start(holo::thread_callback).
			//
			// If the thread could not be created, an appropriate error will be
			// pushed. Query holo::get_last_exception() after invocation.
			void start();
			
			// Begins executing a thread using the provided callback.
			//
			// If the thread already has a callback (provided in the constructor) or
			// the thread has already started, this method will push
			// holo::exception::invalid_operation.
			// 
			// If the provided callback is NULL, then this method will
			// push holo::exception::invalid_argument.
			//
			// If the thread could not be created, an appropriate error will be
			// pushed. Query holo::get_last_exception() after invocation.
			void start(holo::thread_callback callback, void* userdata);
			
			// Waits until the thread finishes executing and then disposes of the
			// resources allocated by the thread.
			//
			// This method is called automatically by the destructor as well.
			//
			// Returns the thread's exit code if the thread was successfully joined.
			//
			// If this method has previously been called and the thread was joined, 
			// then this method pushes holo::exception::invalid_operation.
			holo::thread_return_status join();
			
			// Enable or disable exception handling when the thread starts.
			//
			// Normally, the exception handler is opt-in. This is because exception
			// handling is a cost that may not be necessary for lightweight threads.
			//
			// A thread can still enable exception handling if disabled at thread
			// startup by calling
			// holo::enable_exceptions(holo::allocator*, holo::exception_code_handler_callback).
			// See the method for more information.
			//
			// The default setting is to disable exceptions.
			//
			// This method must be called before starting the thread, otherwise it
			// will have no effect and holo::exception::invalid_operation will be
			// pushed.
			void set_exceptions_flag(bool enable);
			
			// Sets the default allocator for the thread.
			//
			// Setting the allocator is necessary to enable exception handling.
			//
			// If not provided, then no default allocator will be set on the new
			// thread.
			//
			// A value of NULL is valid and indicates that no default allocator
			// should be used.
			//
			// This method must be called before starting the thread, otherwise it
			// will have no effect and holo::exception::invalid_operation will be
			// pushed.
			void set_allocator(holo::allocator* allocator);
			
			// Gets if the thread object is valid.
			//
			// A thread object can be made invalid from incorrect usage or resource
			// limitations.
			//
			// Attempts to operate on an invalid thread object will fail.
			bool is_valid() const;
		
		protected:
			// Initializes the underlying thread object.
			//
			// This method can safely be called by an implementator class in the
			// constructor, as long as it is the top in the inheritance chain. This
			// can, of course, be forced by marking the implementator as 'final' which
			// is the standard procedure.
			void init_thread();
			
			// Sets an argument flag if 'enable' is true, otherwise unsets the flag if
			// 'enable' is false.
			//
			// For a list of flags, refer to 'core/threading/thread_interface.hpp'
			void set_argument_flag(int flag, bool enable);
			
			// Gets if a flag is enabled.
			//
			// Returns true if the flag provided is enabled, false otherwise.
			bool get_argument_flag(int flag) const;
			
			// Invalidates the thread object. Any further operations that required a
			// valid thread object will fail.
			//
			// This is equivalent of calling holo::thread::set_argument_flag(thread_invalid, true).
			void invalidate();
		
		private:
			// Initializes the thread argument with an optional thread callback.
			void init_argument(holo::thread_callback callback, void* userdata);
			
			// Argument passed to the internal thread callback.
			thread_argument argument;
	};
}

#endif
