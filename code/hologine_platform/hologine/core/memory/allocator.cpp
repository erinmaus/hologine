// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include "core/platform.hpp"
#include "core/memory/allocator.hpp"

const std::size_t holo::allocator::default_alignment;

void* holo::allocator::align_pointer(void* pointer, std::size_t align)
{
	// Maximum potential offset required to align the pointer.
	std::size_t offset = align - 1;

	// Increment by the maximum offset required, then align along the provided
	// power-of-two boundary. For any power of two (2^x), 'x' least-significant
	// bits will have to be zero for the pointer to be aligned.
	//
	// Therefore, for a properly aligned allocation, there may be 'align - 1'
	// bytes of wasted memory, if no other data is necessary for the allocation.
	//
	// It is up to the caller to figure out if the allocation is possible.
	return (void*)(((uintptr_t)pointer + offset)  & ~offset);
}

std::size_t holo::allocator::get_pointer_distance(void* left, void* right)
{
	return (std::size_t)((char*)left - (char*)right);
}

const holo::exception_code holo::exception::out_of_memory =
	holo::exception_code_generator::generate_exception_code("holo_exception_out_of_memory");
