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
* `holo::pool_allocator`, in order to allow for moderately fast
  allocations/deallocations, has an overhead of __48 bytes__ per allocation on
  x86_64 (24 bytes on 32-bit). This needs to be optimized if targetting
  platforms with limited RAM. In the future, look into making use of something
  like jemalloc or dlmalloc for the general purpose allocator.
   * `holo::heap_allocator` introduces another 16 bytes of overhead on 64-bit
     platforms (8 on 32-bit). Although small allocations are discouraged, it is
     still quite a lot of overhead that could be alleviated.