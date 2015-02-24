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
#include "core/platform_windows.hpp"
#include "core/memory/memory_region_base.hpp"

void* holo::memory_region_base::reserve_pages(std::size_t max_pages)
{
	void* memory = VirtualAlloc(nullptr, max_pages * get_page_size(), MEM_RESERVE, PAGE_NOACCESS);
	
	if (memory == nullptr)
	{
		push_exception(exception::platform, GetLastError());
	}
	
	return memory;
}

void holo::memory_region_base::release_pages(void* base, std::size_t index, std::size_t count)
{
	if (!VirtualFree(
		(char*)base + index * get_page_size(),
		count * get_page_size(),
		MEM_RELEASE))
	{
		push_exception(exception::platform, GetLastError());
	}
}

bool holo::memory_region_base::commit_pages(void* base, std::size_t index, std::size_t count)
{
	if (VirtualAlloc(
		(char*)base + index * get_page_size(),
		count * get_page_size(),
		MEM_COMMIT,
		PAGE_READWRITE) == nullptr)
	{
		push_exception(exception::platform, GetLastError());
		
		return false;
	}
	
	return true;
}

void holo::memory_region_base::decommit_pages(void* base, std::size_t index, std::size_t count)
{
	if (!VirtualFree(
		(char*)base + index * get_page_size(),
		count * get_page_size(),
		MEM_DECOMMIT))
	{
		push_exception(exception::platform, GetLastError());
	}
}

std::size_t holo::memory_region_base::get_page_size()
{
	// On Windows (32-bit and 64-bit, x86), the page size is a constant number:
	// 4096 bytes. This, of course, can change in future versions of the operating
	// system, so maybe we should cache the value from GetSystemMetrics() and use
	// that instead.
	return 0x1000u;
}

std::size_t holo::memory_region_base::get_granularity()
{
	// A call to VirtualAlloc is on a 64kb boundary, and thus memory regions
	// must be on 64kb boundaries.
	return 0x10000u;
}
