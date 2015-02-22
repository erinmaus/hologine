#include "core/exception.hpp"
#include "core/threading/thread.hpp"

const holo::thread_return_status holo::thread_return_status_ok = 0;

holo::thread::thread()
{
	init_argument(nullptr, nullptr);
}

holo::thread::thread(holo::thread_callback callback, void* userdata)
{
	// The callback must be valid (otherwise just use the default constructor!).
	if (callback == nullptr)
	{
		push_exception(exception::invalid_argument);
		init_argument(nullptr, nullptr);
	}
	else
	{
		init_argument(callback, userdata);
		create_thread(&argument);
	}
}

holo::thread::~thread()
{
	if (!get_argument_flag(flag_thread_exited))
	{
		join();
	}
}

void holo::thread::start()
{
	// This method can only be called if the thread is valid, not running, and
	// has been created (i.e., a successful invocation of the constructor).
	if (!is_valid() || get_argument_flag(flag_thread_started) || !get_argument_flag(flag_thread_created))
	{
		push_exception(exception::invalid_operation);
	}
	else
	{
		run_thread();
	}
}

void holo::thread::start(holo::thread_callback callback, void* userdata)
{
	// This method can only be called if the holo::thread_base object is valid and
	// no thread has been created.
	if (!is_valid() || get_argument_flag(flag_thread_created))
	{
		push_exception(exception::invalid_operation);
	}
	else if (callback == nullptr)
	{
		push_exception(exception::invalid_argument);
	}
	else
	{
		argument.callback = callback;
		argument.userdata = userdata;
		create_thread(&argument);
	}
}

holo::thread_return_status holo::thread::join()
{
	// This method can only be called if the holo::thread_base object is valid and
	// the underlying thread has been created.
	if (!is_valid() || !get_argument_flag(flag_thread_started))
	{
		push_exception(exception::invalid_operation);
	}
	else
	{
		if (join_thread())
		{
			return argument.return_status;
		}
	}
		
	// The caller should query if this call was successful via
	// holo::thread_base::is_valid().
	return thread_return_status_ok;
}

void holo::thread::set_exceptions_flag(bool enable)
{
	if (!is_valid() && !get_argument_flag(flag_thread_started))
	{
		push_exception(exception::invalid_operation);
	}
	else
	{
		set_argument_flag(flag_enable_exceptions, enable);
	}
}

void holo::thread::set_allocator(holo::allocator* allocator)
{
	if (!is_valid() && !get_argument_flag(flag_thread_started))
	{
		push_exception(exception::invalid_operation);
	}
	else
	{
		argument.allocator = allocator;
	}
}

bool holo::thread::is_valid() const
{
	return !get_argument_flag(flag_thread_invalid);
}

void holo::thread::set_argument_flag(int flag, bool enable)
{
	if (enable)
	{
		argument.flags |= flag;
	}
	else
	{
		argument.flags &= ~flag;
	}
}

bool holo::thread::get_argument_flag(int flag) const
{
	return (argument.flags & flag) == flag;
}

void holo::thread::invalidate()
{
	set_argument_flag(flag_thread_invalid, true);
}

void holo::thread::init_argument(holo::thread_callback callback, void* userdata)
{
	argument.return_status = thread_return_status_ok;
	argument.callback = callback;
	argument.userdata = userdata;
	argument.flags = 0;
	argument.allocator = nullptr;
}
