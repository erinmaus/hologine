// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_MEMORY_STANDARD_ALLOCATOR_PROXY_HPP_
#define HOLOGINE_CORE_MEMORY_STANDARD_ALLOCATOR_PROXY_HPP_

#include <exception>
#include <memory>
#include <type_traits>
#include "core/memory/allocator.hpp"

namespace holo
{
	// Wraps a holo::allocator, allowing it to be used with C++ containers.
	//
	// Rather than rewrite the containers present in the C++ Standard Library,
	// holo::standard_allocator_proxy retrofits the holo::allocator interface
	// into the strange std::allocator mechanics.
	//
	// The provided allocator is expected to have a lifetime greater than the
	// holo::standard_allocator_proxy. Similarly, the
	// holo::standard_allocator_proxy object will not destroy the underlying
	// holo::allocator instance.
	template <class Type>
	class standard_allocator_proxy
	{
		public:
			// Implementation of type aliases.
			typedef Type value_type;

			// Properties of the allocator.
			typedef std::true_type propagate_on_container_copy_assignment;
			typedef std::true_type propagate_on_container_move_assignment;

			// Rebind mechanism.
			template <class Other>
			struct rebind
			{
				typedef standard_allocator_proxy<Other> other;
			};

			// Constructs a proxy to a holo::allocator for use with standard types.
			standard_allocator_proxy(holo::allocator* allocator);

			// Copy constructor.
			standard_allocator_proxy(const holo::standard_allocator_proxy<Type>& other);

			// Copy constructor for the rebind mechanism.
			template <class Other>
			standard_allocator_proxy(const holo::standard_allocator_proxy<Other>& other);

			// Invokes holo::allocator::allocate(size_t, size_t).
			//
			// Default alignment will be used.
			Type* allocate(std::size_t count);

			// Invokes holo::allocate::deallocate(void*).
			void deallocate(Type* type, std::size_t count);

		private:
			holo::allocator* allocator;
	};

	template <class Type>
	standard_allocator_proxy<Type>::standard_allocator_proxy(holo::allocator* allocator) :
		allocator(other.allocator)
	{
		// Nothing.
	}

	template <class Type>
	standard_allocator_proxy<Type>::
		standard_allocator_proxy(const holo::standard_allocator_proxy<Other>& other) :
			allocator(other.allocator)
	{
		// Nothing.
	}

	template <class Type>
	template <class Other>
	standard_allocator_proxy<Type>::
		standard_allocator_proxy(const holo::standard_allocator_proxy<Other>& other) :
			allocator(other.allocator)
	{
		// Nothing.
	}

	template <class Type>
	Type* standard_allocator_proxy<Type>::allocate(size_t count)
	{
		void* pointer = allocator->allocate(count* sizeof(Type));

		if (pointer == nullptr)
		{
			// Terminate the program. Containers don't handle NULL return values from
			// std::allocator::allocate(size_t); they expect a pointer or an exception.
			std::terminate();
		}

		return (Type*)pointer;
	}

	template <class Type>
	standard_allocator_proxy<Type>::deallocate(Type* pointer, size_t count)
	{
		allocator->deallocate(pointer);
	}

	// Comparison operators.
	template <class Type, class Other>
	bool operator ==(
		const holo::standard_allocator_proxy<Type>& a, 
		const holo::standard_allocator_proxy<Other>& b)
	{
		return a.allocator == b.allocator;
	}

	template <class Type, class Other>
	bool operator ==(
		const holo::standard_allocator_proxy<Type>& a, 
		const holo::standard_allocator_proxy<Other>& b)
	{
		return !(a == b);
	}
}

#endif
