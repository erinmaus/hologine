// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_MEMORY_ALLOCATOR_HPP_
#define HOLOGINE_CORE_MEMORY_ALLOCATOR_HPP_

#include <cstddef>
#include <exception>
#include <new>
#include <utility>
#include "core/exception.hpp"

namespace holo
{
	// Base class for all allocators.
	//
	// An allocator provides access to memory. Even though each allocator can
	// have radically different implementation details, they all provide the same
	// support: allocating and deallocating portions of memory.
	class allocator
	{
		allocator(const holo::allocator&) = delete;
		holo::allocator& operator =(const holo::allocator&) = delete;
		
		public:
			// The default platform-specific alignment value, or a valid alternative.
			//
			// This is the value used by holo::allocator::align(size_t, size_t) when
			// no alignment value is provided.
			//
			// Depending on the platform, this value varies, but is compatible with
			// the default alignment requirements of the host. For example, on
			// x86_64, this value should be 16 bytes, because the x86_64 ABI
			// explicitly requires 16 byte alignment for SSE instructions and stack
			// layout, thus resulting in this value (16) being the default alignment
			// for the system's memory allocation methods.
			//
			// Implementation note: on x86, this value is still 16 bytes because of
			// SSE data alignment requirements.
			static const std::size_t default_alignment = 16;
			
			// Default constructor and deconstructor.
			allocator() = default;
			virtual ~allocator() = default;
			
			// Allocates a block of memory.
			//
			// Attempts to allocate a block of memory 'size' bytes large, aligned on
			// 'align' byte boundaries. Alignment must be a power of two.
			//
			// Returns a valid pointer to the newly allocated block of memory on
			// success; otherwise, returns NULL. Failure can occur if there is no
			// available region of memory that is large enough to fit the new block.
			virtual void* allocate(std::size_t size, std::size_t alignment = default_alignment) = 0;
			
			// Deallocates a block of memory.
			//
			// The block of memory must have been previously allocated by the
			// corresponding 'allocate' method. If this condition is not met, then
			// memory corruption can (and most likely will) occur.
			virtual void deallocate(void* pointer) = 0;
			
			// Allocates and constructs an object using the provided alignment, in
			// bytes.
			template <class Type, typename ...Arguments>
			Type* align_construct(std::size_t align, Arguments&&... args)
			{
				void* pointer = allocate(sizeof(Type), align);

				return new(pointer) Type(std::forward<Arguments>(args)...);
			}
			
			// Allocates and constructs an object.
			//
			// This method uses the default alignment; to align the object, use
			// holo::linear_allocator::align_construct(std::size_t, ...).
			template <class Type, typename ...Arguments>
			Type* construct(Arguments&&... args)
			{
				return align_construct<Type>(default_alignment, std::forward<Arguments>(args)...);
			}
			
			// Allocates and constructs an object array, with an optional alignment
			// in bytes.
			template <class Type>
			Type* construct_array(std::size_t length, std::size_t align = default_alignment)
			{
				void* memory = allocate(sizeof(Type) * length + sizeof(std::size_t), align);
				
				// Put the length of the array before the actual array data so we know the
				// length for when the array is deallocated via
				// holo::destruct_array(Type*).
				*((std::size_t*)memory) = length;
				
				// Construct the elements in the array.
				Type* array = (Type*)((std::size_t)memory + 1);
				for (std::size_t i = 0; i < length; ++i)
				{
					new (array + i) Type;
				}
				
				return array;
			}
			
			// Destructs and deallocates an object.
			template <class Type>
			void destruct(Type* object)
			{
				object->~Type();
				
				deallocate(object);
			}
			
			// Destructs and deallocates an object array.
			template <class Type>
			void destruct_array(Type* array)
			{
				std::size_t length = *((std::size_t*)array - 1);
				
				// C++ standard says to destruct objects in reverse order.
				for (std::size_t i = length - 1; i >= 0; --i)
				{
					array[i].~Type();
				}
				
				void* memory = (std::size_t*)array - 1;
				deallocate(memory);
			}
			
			// Aligns a pointer along the provided boundary, returning the
			// pointer adjusted by the necessary alignment.
			//
			// Although not enforced, 'align' should be a power of two.
			static void* align_pointer(void* pointer, std::size_t align);

			// Gets the distance between two pointers, in bytes.
			static std::size_t get_pointer_distance(void* left, void* right);
	};
	
	namespace exception
	{
		// Represents an error where there was not enough memory to finish the
		// operation.
		extern const exception_code out_of_memory;
	}
}

#endif
