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
	#include <cstdlib>
#endif

#include "core/platform.hpp"

namespace holo
{
	// Contains various methods to operate and bits and bytes.
	namespace math
	{
		// Left rotates 'value' by 'shift'.
		inline std::uint32_t bit_rotate_left(std::uint32_t value, std::uint32_t shift)
		{
			#ifdef HOLOGINE_INTRINSICS_MSVC_COMPATIBLE
				return _rotl(value, shift);
			#else
				// Apparently GCC generates rotate instructions without intrinsics if
				// code follows this form.
				return (value << shift) | (value >> (32 - shift));
			#endif
		}

		// Rught rotates 'value' by 'shift'.
		inline std::uint64_t bit_rotate_right(std::uint32_t value, std::uint32_t shift)
		{
			#ifdef HOLOGINE_INTRINSICS_MSVC_COMPATIBLE
				return _rotr(value, shift);
			#else
				return (value >> shift) | (value << (32 - shift));
			#endif
		}

		// Left rotates 'value' by 'shift'.
		inline std::uint64_t bit_rotate_left(std::uint64_t value, std::uint64_t shift)
		{
			#ifdef HOLOGINE_INTRINSICS_MSVC_COMPATIBLE
				return _rotl64(value, shift);
			#else
				return (value << shift) | (value >> (64 - shift));
			#endif
		}

		inline std::uint64_t bit_rotate_right(std::uint64_t value, std::uint64_t shift)
		{
			#ifdef HOLOGINE_INTRINSICS_MSVC_COMPATIBLE
				return _rotr64(value, shift);
			#else
				return (value >> shift) | (value << (64 - shift));
			#endif
		}

		// Returns the index of the most-signficant.
		//
		// The behavior is undefined if value is zero.
		inline std::uint32_t bit_scan_reverse(std::uint32_t value)
		{
			#ifdef HOLOGINE_INTRINSICS_GCC_COMPATIBLE
				return 31 - __builtin_clz(value);
			#elif HOLOGINE_INTRINSICS_MSVC_COMPATIBLE
				return _BitScanReverse(nullptr, value);
			#else
				#error holo::bit_scan_reverse only has intrinsic implementations
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
			#else
				#error holo::bit_scan_reverse only has intrinsic implementations
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

		// Swaps the bytes in 'value'.
		inline std::uint16_t swap_bytes(std::uint16_t value)
		{
			return (value >> 8) | (value << 8);
		}

		// Swaps the bytes in 'value'.
		inline std::uint32_t swap_bytes(std::uint32_t value)
		{
			return (value << 24)
				| ((value >> 24) & 0xff)
				| ((value & 0x0000ff00) << 8)
				| ((value & 0x00ff0000) >> 8);
		}

		// Swaps the bytes in 'value'.
		inline std::uint64_t swap_bytes(std::uint64_t value)
		{
			return (value << 56)
				| ((value << 40) & 0x00ff000000000000ull)
				| ((value << 24) & 0x0000ff0000000000ull)
				| ((value << 8) & 0x000000ff00000000ull)
				| ((value >> 8) & 0x00000000ff000000ull)
				| ((value >> 24) & 0x0000000000ff0000ull)
				| ((value >> 40) & 0x000000000000ff00ull)
				| (value >> 56);
		}
	}
}

#endif
