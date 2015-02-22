// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLGOINE_CORE_MEMORY_MEMORY_REGION_BASE_HPP_
#define HOLOGINE_CORE_MEMORY_MEMORY_REGION_BASE_HPP_

#include <cstddef>
#include "core/memory/allocator.hpp"
#include "core/memory/memory_region_interface.hpp"

namespace holo
{
	// Windows implementation of a memory region, using VirtualAlloc & co.
	class memory_region_base : public memory_region_interface
	{
		public:
			// Implementation.
			void* reserve_pages(std::size_t max_pages) override;
			
			// Implementation.
			void release_pages(void* base, std::size_t index, std::size_t count) override;
			
			// Implementation.
			bool commit_pages(void* base, std::size_t index, std::size_t count) override;
			
			// Implementation.
			void decommit_pages(void* base, std::size_t index, std::size_t count) override;
			
			// Implementation.
			std::size_t get_page_size() const override;
	};
}

#endif
