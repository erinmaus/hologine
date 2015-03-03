// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_THREADING_EVENT_HPP_
#define HOLOGINE_CORE_THREADING_EVENT_HPP_

#include <cstdint>

namespace holo
{
	class event_queue;

	// A unique enumeration for an event.
	//
	// Events should be defined by Hint to ensure each event is given its own
	// unique type.
	typedef std::uint32_t event_type;

	// A header required for all events.
	//
	// An event is required to have holo::event_header as its first member,
	// otherwise the event queue will be unable to properly dispatch the event,
	// resulting in undefined behavior (most likely, a crash).
	struct event_header
	{
		// The unique event enumeration.
		holo::event_type type;

		enum
		{
			// The event is disposed and thus should be deallocated.
			//
			// This flag is set when the event is processed and is returned to the
			// owning thread.
			event_flag_disposed = 0x00000001
		};

		// Flags associated with the event.
		std::uint32_t flags;

		// The event queue of the owning thread.
		//
		// This is different from the queue the event was pushed to.
		holo::event_queue* queue;
	};
}

#endif
