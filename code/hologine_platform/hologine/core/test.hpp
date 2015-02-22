// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_TEST_HPP_
#define HOLOGINE_CORE_TEST_HPP_

// Enable testing only if built with the proper compile-time define.
//
// For some objects, we need to verify that they are internally stable; writing
// a unit test to verify all possible conditions may not be feasible when
// compared to simply validating internal state. Mostly, this is used for
// composed objects (e.g., allocators); that is, objects that should provide a
// simple interface to a complex domain.
//
// For example, allocators should *not* expose the internal data structures to
// normal user code; however, how do we know how and why a method failed, or
// perhaps even crashed, in an otherwise public unit test? Even though these
// objects ares a special exception to a standard unit test standard, they
// should thoroughly be tested, regardless.
#ifdef HOLOGINE_TESTING_ENABLED

// Enables a unit test to access the internal data of a complex class.
//
// The unit test can then declare an object following the form [class]_test,
// which would in turn be able to access the internal members of the provided
// class. For example, HOLOGINE_ENABLE_TEST(foo) would allow a class named
// 'foo_test' to access the internal state of 'foo.'
#define HOLOGINE_ENABLE_INTERNAL_TEST(class_name) friend class_name##_test

// If disabled, any testing scaffolding have a no impact on the behavior of
// code. Sadly, when testing is enabled, there is a small difference in class
// behavior if a class enables internal testing.
#else
// Simple trick to allow the usage of HOLOGINE_ENABLE_TEST with a trailing
// semicolon: simply define a forward declaration to a type. Regardless of it
// being valid or not, the code will behave the same.
#define HOLOGINE_ENABLE_INTERNAL_TEST(class_name) struct class_name##_test_disabled;

#endif

// Nothing.

#endif
