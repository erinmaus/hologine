// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_THREADING_EVENT_QUEUE_HPP_
#define HOLOGINE_CORE_THREADING_EVENT_QUEUE_HPP_

#include <cstddef>
#include <boost/iterator/iterator_facade.hpp>
#include "core/memory/fixed_allocator.hpp"
#include "core/threading/mutex.hpp"
#include "core/threading/scoped_lock.hpp"
#include "core/threading/event.hpp"

namespace holo
{
	// Represents a queue holding events for a specific thread.
	class event_queue final
	{
		class event_node;

		public:
			// Represents an event_queue_iterator.
			class event_queue_iterator : 
				public boost::iterator_facade<
					event_queue_iterator,
					event_header*,
					boost::forward_traversal_tag,
					event_header*>
			{
				friend class boost::iterator_core_access;

				public:
					event_queue_iterator(event_queue* queue, event_node* node);

				private:
					// Implementation.
					holo::event_header* dereference() const;
					void increment();
					bool equal(const event_queue_iterator& other) const;

					event_queue* queue;
					event_node* node;
			};

			// Creates an event queue, allocating 'size' bytes for bookkeeping.
			//
			// 'size' will be rounded up, if necessary, based on memory region
			// rounding requirements.
			event_queue(std::size_t size);

			// Frees all resources associated with the event queue.
			~event_queue();

			// Pushes an event to the queue.
			//
			// Returns true if the event was successfully enqueued, false otherwise.
			// Failure occurs when the maximum queue size has been reached.
			template <class Event>
			bool push(Event* e);

			// Retrieves a forward iterator to the first event in the queue.
			//
			// This operation effectively clears the queue; the next call to
			// holo::event_queue::begin() will only contain events pushed *after*
			// the previous call.
			//
			// The entire range must be iterated over, otherwise events will be
			// discarded and memory will leak. Similarly, only the current iterator is
			// valid. Storing a copy of a stale iterator will result in undefined
			// behavior.
			event_queue_iterator begin();

			// Retrieves a forward iterator representing the end of the queue.
			event_queue_iterator end();

		private:
			// Disposes of an event sent by the owning thread.
			//
			// Events are allocated by the thread that sent them. Therefore, when the
			// receiving thread is finished processing an event, it should inform the
			// sender thread that the event can be deallocated.
			//
			// This method is automatically called when an event is consumed by an
			// iterator.
			//
			// Returns true if the event was successfully enqueued, false otherwise.
			// Failure occurs when the maximum queue size has been reached.
			bool dispose(event_header* e);

			// The allocator used by the event queue to allocate nodes.
			//
			// This is distinct from the allocator used by the thread to allocate
			// event data.
			holo::fixed_allocator allocator;

			// Represents an event in a queue.
			struct event_node
			{
				event_node* next;
				event_header* event;
			};

			// Head of the event queue.
			event_node* event_node_list_head;

			// Tail of the event queue.
			event_node* event_node_list_tail;

			// Synchronization object.
			holo::mutex mutex;
	};

	template <class Event>
	bool event_queue::push(Event* e)
	{
		holo::scoped_lock lock(mutex);

		event_node* node = allocator.construct<event_node>();
		if (node == nullptr)
		{
			return false;
		}

		node->next = nullptr;
		node->event = (event_header*)e;

		if (event_node_list_tail != nullptr)
		{
			event_node_list_tail->next = node;
		}

		event_node_list_tail = node;
		
		return true;
	}
}

#endif
