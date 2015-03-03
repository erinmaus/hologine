// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include "core/memory/fixed_allocator.hpp"
#include "core/threading/mutex.hpp"
#include "core/threading/event.hpp"
#include "core/threading/event_queue.hpp"

holo::event_queue::event_queue_iterator::event_queue_iterator(
	holo::event_queue* queue,
	event_node* node) :
		queue(queue),
		node(node)
{
	// Nothing.
}

holo::event_header* holo::event_queue::event_queue_iterator::dereference() const
{
	return node->event;
}

void holo::event_queue::event_queue_iterator::increment()
{
	event_node* next = node->next;

	// Dispose of the event.
	node->event->queue->dispose(node->event);

	// Deallocate current node.
	{
		holo::scoped_lock lock(queue->mutex);
		queue->allocator.deallocate(node);
	}

	// Advance.
	node = next;
}

bool holo::event_queue::event_queue_iterator::equal(const event_queue_iterator& other) const
{
	return queue == other.queue && node == other.node;
}

holo::event_queue::event_queue(std::size_t size) :
	allocator(size, sizeof(event_node)),
	event_node_list_head(nullptr),
	event_node_list_tail(nullptr)
{
	// Nothing.	
}

holo::event_queue::~event_queue()
{
	// Nothing.
	//
	// We can't do anything in the event of leaks because we just dispatch events.
	// At best we could assert, since the queue should be empty.
}

holo::event_queue::event_queue_iterator holo::event_queue::begin()
{
	holo::scoped_lock lock(mutex);

	event_node* node = event_node_list_head;

	// As explained in the header, iteration resets the queue.
	event_node_list_head = event_node_list_tail = nullptr;

	// Return the iterator.
	return event_queue_iterator(this, node);
}

holo::event_queue::event_queue_iterator holo::event_queue::end()
{
	// Event queue iterators are forward iterators. We don't have to worry about
	// pointing to the proper 'just-after-end' of the queue.
	return event_queue_iterator(this, nullptr);
}

bool holo::event_queue::dispose(event_header* e)
{
	event_header* header = (event_header*)e;
	holo_assert(header->queue == this);

	header->flags |= holo::event_header::event_flag_disposed;

	return push(e);
}
