// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_MEMORY_MEMORY_ARENA_HPP_
#define HOLOGINE_CORE_MEMORY_MEMORY_ARENA_HPP_

#include <cstddef>
#include "core/memory/memory_region.hpp"

namespace holo
{
	class allocator;

	// Reserves a large portion of virtual memory and divides it up for
	// holo::allocator instances.
	class memory_arena_pool final
	{
		memory_arena_pool(const memory_arena_pool&) = delete;
		memory_arena_pool& operator =(const memory_arena_pool&) = delete;
		
		public:
			// Represents a free node.
			struct allocator_free_node
			{
				// Next free node.
				allocator_free_node* next;

				// Previous free node.
				allocator_free_node* previous;

				// Size of the current free node.
				std::size_t size;
			};

			// Represents a region of memory returned by the arena.
			struct arena_record
			{
				// Pointer to the base of the memory region.
				void* base;

				// The allocator currently using the arena, or NULL if this arena is
				// free.
				holo::allocator* allocator;

				// Number of free nodes.
				std::size_t free_node_count;

				// Pointer to the first free node.
				allocator_free_node* free_node_list;

				// Pointer to the next arena.
				arena_record* next;

				// Pointer to the previous arena.
				//
				// Implementation note: this is unused by the arena pool.
				arena_record* previous;
			};

			// Reserves 'arena_count_hint' arenas of 'arena_size_hint' size.
			memory_arena_pool(std::size_t arena_size_hint, std::size_t arena_count_hint);

			// Releases the arenas.
			~memory_arena_pool();

			// Reserves a list of arenas for use.
			//
			// Returns the number of arenas reserved. If this number is less than
			// the requested region count, then this method could not successfully
			// reserve enough arenas. In such a case, an exception will be at the
			// top of the stack indicating the reason.
			std::size_t reserve(std::size_t count);
			
			// Returns an arena, creating one if necessary.
			arena_record* take_arena();
			
			// Releases an arena previously allocated by the pool.
			void give_arena(arena_record* record);

			// Gets the arena a pointer resides in.
			arena_record* get_arena(void* pointer);

			// Gets the size of an arena.
			std::size_t get_arena_size() const;

			// Gets the number of arenas currently allocated.
			std::size_t get_arena_count() const;

			// Gets the reserved number of arenas.
			//
			// This is equivalent to the count hint provided in the constructor.
			std::size_t get_reserved_arena_count() const;
		
		private:
			// Allocates a new arena.
			bool allocate_arena();

			// The backing memory region.
			holo::memory_region memory_region;

			// Array of arena records.
			arena_record* records;

			// Next free arena.
			arena_record* next_free_arena;

			// Pointer to the arena pool.
			void* arena_pool;

			// Size of an individual arena.
			std::size_t arena_size;
			
			// Total number of arenas reserved.
			std::size_t arena_reserved;

			// Last allocated arena.
			std::size_t arena_count;
	};
}

#endif
