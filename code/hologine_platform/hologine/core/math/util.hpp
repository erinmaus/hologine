// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_MATH_UTIL_HPP_
#define HOLOGINE_CORE_MATH_UTIL_HPP_

#include <cstdint>

namespace holo
{
	// Includes various utility methods, such as rounding, that are absent from
	// the standard library.
	namespace math
	{
		// Rounds a value up to the nearest factor.
		template <class Numeric>
		Numeric round_up(Numeric value, Numeric factor)
		{
			return value + (factor - 1) - ((value - 1) % factor);
		}
		
		// Returns the nearest rounded multiple of a value.
		//
		// This is essentially the same as round_up(value, factor) / factor.
		template <class Numeric>
		Numeric multiple_of(Numeric value, Numeric factor)
		{
			return round_up(value, factor) / factor;
		}
		
		// Gets a boolean indicating if a value is a power of two.
		template <class UnsignedInteger>
		bool is_power_of_two(UnsignedInteger value)
		{
			// The algorithm here is simple:
			//
			// A power of two only has one bit set (e.g., 0x1000 -> 0001 0000 0000 0000).
			// Subtract one from the value in question. In the case of 0x1000, we get
			// 0000 1111 1111 1111. Now perform a bitwise AND with the original value:
			//
			//     value'1 -> 0001 0000 0000 0000
			// AND value'2 -> 0000 1111 1111 1111
			//             == 0000 0000 0000 0000
			//
			// Thus, a power of two will be zero when AND'ed with a value one less
			// than the power of two being tested.
			//
			// Now try for an obvious non-power of two, say 0x03FC:
			//
			//     value'1 -> 0000 0011 1111 1100
			// AND value'2 -> 0000 0011 1111 1011
			//             == 0000 0011 1111 1011
			//
			// Any other non-power of two will be the same.
			//
			// Simple!
			return value != 0 && !(value & ~(value - 1));
		}
		
		// Gets the next-highest power-of-two of a 32-bit unsigned integer.
		uint32_t next_power_of_two(uint32_t value)
		{
			--value;
			
			value |= value >> 1;
			value |= value >> 2;
			value |= value >> 4;
			value |= value >> 8;
			value |= value >> 16;
			
			return ++value;
		}
		
		// Gets the next highest power-of-two of a 64-bit unsigned integer.
		uint64_t new_power_of_two(uint64_t value)
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
	}
}

#endif
