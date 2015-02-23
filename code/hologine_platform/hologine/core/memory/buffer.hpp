// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_MEMORY_BUFFER_HPP_
#define HOLOGINE_CORE_MEMORY_BUFFER_HPP_

#include <cstddef>
#include "core/memory/allocator.hpp"

namespace holo
{
	// A buffer represents a portion of memory whose size is defined at compile
	// time.
	//
	// Buffers are pretty useful in bootstrapping objects or arrays of objects,
	// which is often the case in memory allocators.
	template <std::size_t Size, std::size_t Alignment = 16>
	class buffer
	{
		public:
			// Gets a pointer to the buffer.
			//
			// The return value will be aligned to the boundary specified in the
			// template arguments.
			void* get();

		private:
			char data[Size + Alignment - 1];
	};

	template <std::size_t Size, std::size_t Alignment>
	void* buffer<Size, Alignment>::get()
	{
		return holo::allocator::align_pointer(data, Alignment);
	}
}

#endif
