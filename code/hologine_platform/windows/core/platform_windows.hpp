// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_PLATFORM_WINDOWS_HPP_
#define HOLOGINE_CORE_PLATFORM_WINDOWS_HPP_

// Simply include the Windows header...
#define WIN32_LEAN_AND_MEAN

#ifdef WINVER
	#undef WINVER
#endif

#ifdef _WIN32_WINNT
	#undef _WIN32_WINNT
#endif

#define WINVER 0x0600
#define _WIN32_WINNT 0x0600

#include <windows.h>

// ...and clear up any namespace clashes.
// For example, 'near' and 'far' could pollute math or algorithms with similarly
// named variables, causing bizarre compile errors.
#undef near
#undef far

#undef exception_code

#endif
