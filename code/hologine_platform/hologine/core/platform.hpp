// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_PLATFORM_HPP_
#define HOLOGINE_CORE_PLATFORM_HPP_

// Platform-specific header to include platform-specific things like intptr_t
// in the event the compiler or standard library does not include it.
//
// Not to be confused with platform_*.hpp (e..g, platform_windows.hpp). The
// specific platform headers are used by the platform interop code only. They
// should not be used externally.
//
// For now, assume uintptr_t and intptr_t are supported. If not, add a step to
// the build system to generate it.
#include <cstdint>

namespace holo
{
	typedef std::uintptr_t unsigned_pointer;
	typedef std::intptr_t signed_pointer;
}

#endif
