// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations, most
// notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_PLATFORM_TEST_ALLOCATOR_HPP_
#define HOLOGINE_PLATFORM_TEST_ALLOCATOR_HPP_

#include <cstdlib>
#include "core/memory/allocator.hpp"

/// Simple allocator over malloc/free.
class test_allocator final : public holo::allocator
{
	public:
		void* allocate(std::size_t size, std::size_t align = default_align)
		{
			void* pointer = std::malloc(size + sizeof(void*) + (align - 1));
			void* aligned_pointer = align_pointer((char*)pointer + sizeof(void*), align);

			*((void**)aligned_pointer - 1) = pointer;

			return aligned_pointer;
		}

		void deallocate(void* pointer)
		{
			free(*((void**)pointer - 1));
		}
};

#endif
