// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_MATH_BITS_HPP_
#define HOLOGINE_CORE_MATH_BITS_HPP_

#ifdef HOLOGINE_INTRINSICS_MSVC_COMPATIBLE
	#include <intrin.h>
#endif

#include "core/platform.hpp"

namespace holo
{
	namespace math
	{
		// Returns the index of the most-signficant.
		//
		// The behavior is undefined if value is zero.
		inline std::uint32_t bit_scan_reverse(std::uint32_t value)
		{
			#ifdef HOLOGINE_INTRINSICS_GCC_COMPATIBLE
				return 31 - __builtin_clz(value);
			#elif HOLOGINE_INTRINSICS_MSVC_COMPATIBLE
				return _BitScanReverse(nullptr, value);
			#endif
		}

		// Returns the index of the most-signficant.
		//
		// The behavior is undefined if value is zero.
		inline std::uint64_t bit_scan_reverse(std::uint64_t value)
		{
			#ifdef HOLOGINE_INTRINSICS_GCC_COMPATIBLE
				return 63 - __builtin_clzll(value);
			#elif HOLOGINE_INTRINSICS_MSVC_COMPATIBLE
				return _BitScanReverse64(nullptr, value);
			#endif
		}

		// Calculates the log2 of a 32-bit integer using bitwise operations.
		//
		// The result is rounded down. Therefore, bit_log2(2 ^ X) is greater than
		// bit_log2(2 ^ X - 1), where '^' represents 'to the power of'.
		//
		// This just aliases holo::bit_scan_reverse(uint32_t) if value > 0.
		inline std::uint32_t bit_log2(std::uint32_t value)
		{
			// If the value is 0, the result of holo::bit_scan_reverse() is undefined.
			// However, ensuring the least significant bit is set will not alter
			// result of the method for any value, since the result is floored.
			return bit_scan_reverse(value | 1);
		}

		// Calculates the log2 of a 64-bit integer using bitwise operations.
		//
		// This method is identical to holo::bit_log2(uint32_t) otherwise.
		inline std::uint64_t bit_log2(std::uint64_t value)
		{
			return bit_scan_reverse(value | 1);
		}
	}
}

#endif
