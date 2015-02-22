// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include <utility>
#include "core/memory/memory_region_free_list.hpp"

holo::memory_region_free_list::memory_region_free_list(std::size_t size) :
	free_list_head(nullptr),
	user_size(size),
	size(size + sizeof(memory_region_header))
{
	// Nothing.
}

holo::memory_region_free_list::~memory_region_free_list()
{
	reset();
}

std::size_t holo::memory_region_free_list::reserve(std::size_t count)
{
	std::size_t reserved = 0;
	while (count-- > 0 && allocate_new_region())
		++reserved;

	return reserved;
}

void* holo::memory_region_free_list::pop()
{
	memory_region_header* node = nullptr;
	
	if (free_list_head == nullptr && !allocate_new_region())
	{
		// Some platform-specific reason has resulted in us unable to allocate a new
		// region.
		return nullptr;
	}
	else
	{
		node = free_list_head;
		
		// Update the head of the free list.
		free_list_head = node->next;
	}
	
	// We only need to preserve the holo::memory_region in the header.
	// The other data (well, in this current implementation, the next pointer) is
	// only used when the node is stored in the free list.
	return (char*)node + sizeof(memory_region);
}

void holo::memory_region_free_list::push(void* region_base)
{
	// Adjust the pointer to get to the base of the virtual memory, where the
	// header begins. Keep in mind we don't adjust by the complete
	// holo::memory_region_header, and rather just the size of a
	// holo::memory_region; this is because the rest of the header is discarded
	// when the region is considered 'in-use'.
	memory_region_header* node =
		(memory_region_header*)((char*)region_base - sizeof(memory_region));
	
	// Store it at the beginning of the free list. This means that, unless another
	// memory region is pushed on to the free list, this will be the next memory
	// region returned by holo::memory_region_free_list::pop().
	node->next = free_list_head;
	free_list_head = node;
}

void holo::memory_region_free_list::reset()
{
	// Iterate over the free list and destroy all holo::memory_region instances.
	memory_region_header* node = free_list_head;
	while (node != nullptr)
	{
		// First, we need to move the holo::memory_region from its current location;
		// as it is now, the state (i.e., the holo::memory_region) is stored inside
		// the virtual memory region! So invoking the destructor as-is will lead to
		// undefined behavior. All we know is that calling the destructor will
		// release all memory associated with the virtual memory region (therefore,
		// if the instance tries to access any internal state after releasing the
		// virtual memory to system, there will be an access violation).
		memory_region current_memory_region = std::move(node->memory_region);
		
		// Update the current node pointer to point to the next in the list before
		// we release the node pointer's underlying storage.
		node = node->next;
		
		// Free the memory.
		current_memory_region.~memory_region();
	}
	
	free_list_head = nullptr;
}

std::size_t holo::memory_region_free_list::get_memory_region_size() const
{
	return user_size;
}

bool holo::memory_region_free_list::allocate_new_region()
{
	memory_region_header* node;
	memory_region new_memory_region(size);
	
	// Claim the memory.
	void* pointer = new_memory_region.claim();
	if (pointer == nullptr)
	{
		// Bail out and propogate the underlying error (on the stack) to the caller.
		return false;
	}
	
	// Prepare to move the memory_region into its virtual memory region!
	// Inception...!
	//
	// First, we need to construct an empty holo::memory_region at the beginning
	// of the virtual memory region since it is not a POD. This will ensure that
	// all compiler-infrastructure (i.e., stuff we can't access
	// [i.e., the vtable and run-time type information, if enabled]) is in place.
	//
	// Note: the holo::memory_region is stored at the base of the virtual memory
	// region, so just use placement new to construct an empty memory region at
	// that location.
	new(pointer) memory_region();
	
	// Next, invoke the move constructor.
	node = (memory_region_header*)pointer;
	node->memory_region = std::move(new_memory_region);

	// Now add it to the free list. No need to duplicate the logic.
	push((char*)node + sizeof(memory_region));

	// The memory region was successfuly created.
	return true;
}
