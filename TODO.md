# Soon
* Write unit tests for:
  * `holo::pool_allocator` (high priority)
  * `holo::linear_allocator` (high priority)
  * `holo::memory_region` (medium priority)
  * `holo::string_builder` (low priority)
* Finish the generic heap allocator.
* Implement `holo::memory_region::shrink(std::size_t)` to allow more fine-tuned
  control over the committed portions of a virtual memory region.

# Known issues
* `holo::pool_allocator` has yet to be tested for validity of implementation. In
  fact, it probably doesn't work as described at all (i.e., it crashes...
  everywhere!).