// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_TEXT_FORMAT_HPP_
#define HOLOGINE_CORE_TEXT_FORMAT_HPP_

#include <cstdio>
#include "core/text/string_builder.hpp"

namespace holo
{
	namespace format
	{
		// Flags shared among various formatters.
		namespace flags
		{
			// Numeric flags.
			enum
			{
				// Forces the display of a sign.
				//
				// Normally, only a negative sign is displayed.
				numeric_force_sign = 0x1,

				// Forces the display of either a decimal point or hex-specifier.
				//
				// Decimal points are otherwise only displayed when necessary (i.e.,
				// there is fractional data).
				//
				// A hex-specifier will only be displayed with this flag enabled.
				numeric_force_special = 0x2
			};

			// Padding mode for numerics.
			enum
			{
				// Pad with spaces, when necessary.
				//
				// This is the default value.
				pad_spaces = 0x0,

				// Pad with zeroes, when necessary.
				pad_zeroes = 0x1
			};

			// Alignment flags.
			enum
			{
				// Right aligns the data within the cell.
				//
				// This is the default value.
				align_right = 0x0,

				// Left aligns the data within the cell.
				align_left = 0x1
			};

			// Case flags.
			enum
			{
				// Prints lowercase, when possible (e.g., hexadecimal data).
				//
				// This is the default value.
				case_lower = 0x0,

				// Prints uppercase, when possible.
				case_upper = 0x1
			};
		}

		namespace modes
		{
			// Possible integer printing modes.
			enum
			{
				// A signed integer.
				//
				// This is the default.
				integer_signed,

				// An unsigned integer.
				integer_unsigned,

				// An unsigned octal integer.
				integer_unsigned_octal,

				// An unsigned hexadecimal integer.
				integer_unsigned_hexadecimal
			};

			// Possible floating-point printing modes.
			enum
			{
				// A fixed-point decimal.
				//
				// This is the default.
				floating_point_decimal,

				// Scientific notation.
				floating_point_scientific_notation,

				// Use the smallest representation of either decimal or scientific
				// notation.
				floating_point_small
			};
		}

		// Base interface for all text formats that are compatible with
		// holo::string_builder.
		struct format_interface
		{
			// Appends the formatted data to a holo::string_builder instance.
			virtual bool append_format(holo::string_builder* builder) const = 0;
		};

		// Formats according to integer specifications.
		template <class Integer>
		struct integer : public format_interface
		{
			public:
				// Formats an integer, 'value', using the provided flags.
				//
				// The function of the flags correspond to the printf family of
				// methods.
				explicit integer(
					Integer value,
					int mode = modes::integer_signed,
					int width = 0,
					int numeric_flags = 0,
					int pad_flags = flags::pad_spaces,
					int align_flags = flags::align_right,
					int case_flags = flags::case_lower);

				// Implementation.
				bool append_format(holo::string_builder* builder) const override;

			private:
				// In order to determine the specific length specifiers for the integer
				// type provided, use template specialization.
				//
				// If no suitable specialization could be found, the deduction will
				// generate a compile time error.
				//
				// 'Unused' is necessary because explicit specialization is not allowed
				// outside of namespace scope. Eurgh.
				template <class I, class Unused = void>
				struct length_specifier {};

				template <class Unused>
				struct length_specifier<int, Unused>
				{
					static const char* spec()
					{
						return "";
					}
				};

				template <class Unused>
				struct length_specifier<unsigned int, Unused>
				{
					static const char* spec()
					{
						return "";
					}
				};

				template <class Unused>
				struct length_specifier<long int, Unused>
				{
					static const char* spec()
					{
						return "l";
					}
				};

				template <class Unused>
				struct length_specifier<long unsigned int, Unused>
				{
					static const char* spec()
					{
						return "l";
					}
				};

				template <class Unused>
				struct length_specifier<std::size_t, Unused>
				{
					static const char* spec()
					{
						return "z";
					}
				};

				// The integer value to print.
				Integer value;

				// The width of the integer when printed.
				int width;

				// This is more then plenty.
				enum { maximum_format_string_length = 16 };

				// The internal format string built from the provided flags.
				char format_string[maximum_format_string_length];
		};

		template <class Integer>
		integer<Integer>::integer(
			Integer value,
			int mode,
			int width,
			int numeric_flags,
			int pad_flags,
			int align_flags,
			int case_flags) :
				value(value),
				width(width)
		{
			int index = 0;

			format_string[index++] = '%';

			if (align_flags & flags::align_left)
				format_string[index++] = '-';

			if (numeric_flags & flags::numeric_force_sign)
				format_string[index++] = '+';

			if (numeric_flags & flags::numeric_force_special)
				format_string[index++] = '#';

			if (pad_flags & flags::pad_zeroes)
				format_string[index++] = '0';

			format_string[index++] = '*';

			const char* spec = length_specifier<Integer>::spec();
			for (int i = 0; spec[i]; ++i)
			{
				format_string[index++] = spec[i];
			}

			switch (mode)
			{
				default:
				case modes::integer_signed:
					format_string[index++] = 'd';
					break;
				case modes::integer_unsigned:
					format_string[index++] = 'u';
					break;
				case modes::integer_unsigned_octal:
					format_string[index++] = 'o';
					break;
				case modes::integer_unsigned_hexadecimal:
					format_string[index++] = (case_flags & flags::case_upper) ? 'X' : 'x';
					break;
			}

			format_string[index] = 0;
		}

		template <class Integer>
		bool integer<Integer>::append_format(holo::string_builder* builder) const
		{
			int count = std::snprintf(nullptr, 0, format_string, width, value);

			if (count > 0)
			{
				char* buffer = builder->request_buffer((std::size_t)count + 1);

				if (buffer)
				{
					std::snprintf(buffer, (std::size_t)count + 1, format_string, width, value);

					return true;
				}
			}

			return false;
		}

		// Formats according to floating-point specifications.
		template <class Float>
		struct floating_point : public format_interface
		{
			public:
				// Formats a floating point, 'value', using the provided flags.
				//
				// The function of the flags correspond to the printf family of
				// methods.
				explicit floating_point(
					Float value,
					int mode = modes::floating_point_decimal,
					int width = 6,
					int numeric_flags = 0,
					int pad_flags = flags::pad_spaces,
					int align_flags = flags::align_right,
					int case_flags = flags::case_lower);

				// Implementation.
				bool append_format(holo::string_builder* builder) const override;

			private:
				// The floating point value to print.
				Float value;

				// The width of the floating point when printed.
				int width;

				// This is more then plenty.
				enum { maximum_format_string_length = 16 };

				// The internal format string built from the provided flags.
				char format_string[maximum_format_string_length];
		};

		template <class Float>
		floating_point<Float>::floating_point(
			Float value,
			int mode,
			int width,
			int numeric_flags,
			int pad_flags,
			int align_flags,
			int case_flags) :
				value(value),
				width(width)
		{
			int index = 0;

			format_string[index++] = '%';

			if (align_flags & flags::align_left)
				format_string[index++] = '-';

			if (numeric_flags & flags::numeric_force_sign)
				format_string[index++] = '+';

			if (numeric_flags & flags::numeric_force_special)
				format_string[index++] = '#';

			if (pad_flags & flags::pad_zeroes)
				format_string[index++] = '0';

			format_string[index++] = '.';
			format_string[index++] = '*';

			switch (mode)
			{
				default:
				case modes::floating_point_decimal:
					format_string[index++] = (case_flags & flags::case_upper) ? 'F' : 'f';
					break;
				case modes::integer_unsigned:
					format_string[index++] = (case_flags & flags::case_upper) ? 'E' : 'e';
					break;
				case modes::integer_unsigned_octal:
					format_string[index++] = (case_flags & flags::case_upper) ? 'G' : 'g';
					break;
			}

			format_string[index] = 0;
		}

		template <class Float>
		bool floating_point<Float>::append_format(holo::string_builder* builder) const
		{
			int count = std::snprintf(nullptr, 0, format_string, width, value);

			if (count > 0)
			{
				char* buffer = builder->request_buffer((std::size_t)count + 1);

				if (buffer)
				{
					std::snprintf(buffer, (std::size_t)count + 1, format_string, width, value);

					return true;
				}
			}

			return false;
		}
	}
}

#endif
