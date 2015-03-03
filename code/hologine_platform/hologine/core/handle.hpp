// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_HANDLE_HPP_
#define HOLOGINE_CORE_HANDLE_HPP_

#include "core/platform.hpp"
#include "core/math/bits.hpp"

namespace holo
{
	// Represents a handle.
	//
	// Handles are opaque data types used by the various systems to reference
	// objects that meet certain requirements.
	//
	// A handle should be used when:
	//
	// 1) An object has a limited lifespan. Once the lifespan of the object is up,
	//    references should be invalidated. However, if the object is referenced
	//    in some indeterminable (because of speed or complexity) manner, stale
	//    references should not cause undefined behavior.
	//
	// 2) An object would otherwise be referenced by an index (i.e., it is stored
	//    in a physically linear container).
	//
	// 3) An object or references to an object must be able to be serialized to
	//    some source (e.g., a file on disk or a network stream).
	//
	// 4) An object is exclusively owned by its system.
	//
	// Encoded in handles are four fields: type, age, scope, and index.
	//
	// Type represents the system that manages the underlying object represented
	// by the handle.
	//
	// Age is a value to determine if a handle is out of date. If a handle
	// otherwise is valid for an object, but the ages are different, then that
	// means the handle is stale and should be discarded.
	//
	// Scope is specific to the system. Generally, it should be used to signify
	// the object subtype if a system can have derived objects.
	//
	// Lastly, index should represent the index of the object.
	//
	// Each of these values, other than 'type', can be configured on per-system
	// basis, depending on the needs of the system. For example, if a system only
	// stores objects of one type, then scope can be ignored, allowing for more
	// precision when encoding the other aspects of the handle.
	//
	// For more information on encoding and decoding handles, see below.
	typedef std::uint64_t handle;

	namespace handle_detail
	{
		// The number of bits available for use in a handle.
		//
		// This value is considerably generous. Changing it would invalidate all
		// handle encoders defined previously. Hence... leave this value alone.
		const std::size_t handle_bits = 64;

		// The bits used to encode a handle type.
		//
		// Like 'handle_bits', this value should probably never be altered. Doing so
		// will break compatibility with existing code.
		const std::size_t handle_type_bits = 8;

		// Type is stored in the eight most significant bits.
		const std::size_t handle_type_shift = 56;

		// The maximum value of a handle type.
		//
		// This is the upper limit to the number of systems that can generate
		// handles specifically for their use.
		//
		// A handle type of '0' is reserved to represent an invalid handle.
		const std::size_t handle_max_type = 0xff;

		// The type mask.
		const std::size_t handle_type_mask = 0xff;
	}

	// Represents a type that can encode and decode handles with the provided
	// parameters.
	//
	// 'Type' is the literal type of the handle, while the other arguments
	// represent the number of bits used for the respective values.
	//
	// All bits of a handle should be used if possible. The actual sizes of each
	// encoded value should be used to maximum effectiveness by the system.
	//
	// Keep in mind that changing these values may invalidate all handles from
	// previous serialized states.
	template <std::size_t Type, std::size_t Age, std::size_t Scope, std::size_t Index>
	struct handle_definition
	{
		private:
			// The index is stored from bits 0 to 'Index', thus its shift value is 0
			// regardless of arguments.
			static const std::size_t index_shift = 0;

			// The mask value for index.
			static const std::size_t index_mask = math::mask<Index>::value;
			
			// The scope immediately comes after the index, thus its shift value is
			// 'Index' bits.
			static const std::size_t scope_shift = Index;

			// The mask value for scope.
			static const std::size_t scope_mask = math::mask<Scope>::value;

			// Age is stored after scope, so accumulate the bit counts.
			static const std::size_t age_shift = Index + Scope;

			// The mask value for age.
			static const std::size_t age_mask = math::mask<Age>::value;

		public:
			// The handle type, as defined in the template parameter.
			static const std::size_t type = Type;

			// The maximum index value.
			static const std::size_t max_index = index_mask;

			// The maximum scope value.
			static const std::size_t max_scope = scope_mask;

			// The maximum age value.
			static const std::size_t max_age = age_mask;

			// Encodes a handle.
			//
			// No error checking is done on the provided values. Values out of range
			// will simply produce unexpected results.
			holo::handle encode(std::size_t age, std::size_t scope, std::size_t index);

			// Gets if the provided handle is valid for the handle definition.
			//
			// This should be used before decoding any values, if robustness is
			// necessary.
			bool is_type(holo::handle handle);

			// Decodes the handle age.
			//
			// This method does not verify the handle type is correct. Ensure validity
			// by using holo::handle_definition::is_type(holo::handle) first.
			std::size_t decode_age(holo::handle handle);

			// Decodes the handle scope.
			//
			// This method does not verify the handle type is correct. Ensure validity
			// by using holo::handle_definition::is_type(holo::handle) first.
			std::size_t decode_scope(holo::handle handle);

			// Decodes the handle index.
			//
			// This method does not verify the handle type is correct. Ensure validity
			// by using holo::handle_definition::is_type(holo::handle) first.
			std::size_t decode_index(holo::handle handle);

			// Utility to increment age within range.
			//
			// Age values start at 1 and wrap when they reach max_age.
			std::size_t increment_age(std::size_t age);
	};

	template <std::size_t Type, std::size_t Age, std::size_t Scope, std::size_t Index>
	holo::handle handle_definition<Type, Age, Scope, Index>::encode(
		std::size_t age,
		std::size_t scope,
		std::size_t index)
	{
		std::size_t encoded_type = (Type & holo::handle_detail::handle_type_mask) << holo::handle_detail::handle_type_shift;
		std::size_t encoded_age = (age & age_mask) << age_shift;
		std::size_t encoded_scope = (scope & scope_mask) << scope_shift;
		std::size_t encoded_index = (index & index_mask) << index_shift;

		return encoded_type | encoded_age | encoded_scope | encoded_index;
	}

	template <std::size_t Type, std::size_t Age, std::size_t Scope, std::size_t Index>
	bool handle_definition<Type, Age, Scope, Index>::is_type(holo::handle handle)
	{
		std::size_t handle_type = (handle >> holo::handle_detail::handle_type_shift) & holo::handle_detail::handle_type_mask;

		return handle_type == Type;
	}

	template <std::size_t Type, std::size_t Age, std::size_t Scope, std::size_t Index>
	std::size_t handle_definition<Type, Age, Scope, Index>::decode_age(holo::handle handle)
	{
		return (handle >> age_shift) & age_mask;
	}

	template <std::size_t Type, std::size_t Age, std::size_t Scope, std::size_t Index>
	std::size_t handle_definition<Type, Age, Scope, Index>::decode_scope(holo::handle handle)
	{
		return (handle >> scope_shift) & scope_mask;
	}

	template <std::size_t Type, std::size_t Age, std::size_t Scope, std::size_t Index>
	std::size_t handle_definition<Type, Age, Scope, Index>::decode_index(holo::handle handle)
	{
		return (handle >> index_shift) & index_mask;
	}

	template <std::size_t Type, std::size_t Age, std::size_t Scope, std::size_t Index>
	std::size_t handle_definition<Type, Age, Scope, Index>::increment_age(std::size_t age)
	{
		return (age % max_age) + 1;
	}
}

#endif
