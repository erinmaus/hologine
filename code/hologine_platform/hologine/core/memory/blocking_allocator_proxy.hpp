// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_MEMORY_BLOCKING_ALLOCATOR_PROXY_HPP_
#define HOLOGINE_CORE_MEMORY_BLOCKING_ALLOCATOR_PROXY_HPP_

#include "core/memory/allocator.hpp"
#include "core/threading/mutex.hpp"

namespace holo
{
	// A naive thread-safe allocator.
	//
	// Calls to allocate and deallocate will block until the mutex can be claimed
	// by the current thread.
	class blocking_allocator_proxy : public allocator
	{
		public:
			// Constructs a blocking allocator.
			blocking_allocator_proxy(holo::allocator* allocator);

			// Implementation.
			void* allocate(std::size_t size, std::size_t alignment = default_alignment);

			// Implementation.
			void deallocate(void* pointer);

		private:
			// Underlying allocator to use.
			holo::allocator* allocator;

			// It's a mutex...
			holo::mutex mutex;
	};
}

#endif
