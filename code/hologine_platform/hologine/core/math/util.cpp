#include "core/math/util.hpp"

std::uint32_t holo::math::next_power_of_two(std::uint32_t value)
{
	--value;
	
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	
	return ++value;
}

std::uint64_t holo::math::new_power_of_two(std::uint64_t value)
{
	--value;
	
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	value |= value >> 32;
	
	return ++value;
}
