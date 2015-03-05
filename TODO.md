# Soon
* Write unit tests for: EVERYTHING!!
* Implement `holo::memory_region::shrink(std::size_t)` to allow more fine-tuned
  control over the committed portions of a virtual memory region.
* Write versions of `holo::math::bit_scan_reverse` that do not depend on 
  intrinsics.

# Known issues
* `holo::heap_allocator` does not support large allocations (that is, an
  allocation greater than the maximum pool size specified in constructor). This
  behavior may be kept, because large allocations are generally specific to
  certain tasks that could likely benefit from a more efficient or specialized
  allocation strategy (resource loading/storage, entity management, etc).
