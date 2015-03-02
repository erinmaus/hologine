// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include "core/exception.hpp"
#include "core/memory/blocking_allocator_proxy.hpp"
#include "core/threading/scoped_lock.hpp"

holo::blocking_allocator_proxy::blocking_allocator_proxy(holo::allocator* allocator) :
	allocator(allocator)
{
	holo_assert(allocator != nullptr);
}

void* holo::blocking_allocator_proxy::allocate(std::size_t size, std::size_t alignment)
{
	holo::scoped_lock lock(mutex);

	return allocator->allocate(size, alignment);
}

void holo::blocking_allocator_proxy::deallocate(void* pointer)
{
	holo::scoped_lock lock(mutex);

	return allocator->deallocate(pointer);
}
