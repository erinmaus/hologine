// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_MATH_VECTOR4_HPP_
#define HOLOGINE_CORE_MATH_VECTOR4_HPP_

namespace holo
{
	class matrix;

	// A four-dimensional vector.
	struct vector4
	{
		/// Generates an empty vector.
		vector4();
		
		/// Generates a vector from four scalars.
		vector4(float x, float y, float z, float w);
		
		/// Adds two vectors.
		holo::vector4& operator +=(const holo::vector4& other);
		
		/// Adds a scalar to the components of a vector.
		holo::vector4& operator +=(float other);
		
		/// Subtracts two vectors.
		holo::vector4& operator -=(const holo::vector4& other);
		
		/// Subtracts a scalar from the components of a vector.
		holo::vector4& operator -=(float other);
		
		/// Multiplies two vectors.
		holo::vector4& operator *=(const holo::vector4& other);
		
		/// Multiplies the components of a vector by a scalar.
		holo::vector4& operator *=(float other);
		
		/// Divides a vector by another.
		holo::vector4& operator /=(const holo::vector4& other);
		
		/// Divides the components of a vector by a scalar.
		holo::vector4& operator /=(float other);
		
		/// Negates a vector.
		holo::vector4 operator -() const;
		
		/// Calculates the length squared of a vector.
		float calculate_length_squared() const;
		
		/// Calculates the length of a vector.
		float calculate_length() const;
		
		/// Returns the normal of the provided vector.
		static holo::vector4 normalize(const holo::vector4& vector);
		
		/// Returns the dot product of the provided vectors.
		static float dot(const holo::vector4& a, const holo::vector4& b);
		
		/// Calculates the distance between two vectors.
		static float distance(const holo::vector4& from, const holo::vector4& to);
		
		/// Transforms a vector by a matrix.
		static holo::vector4 transform(const holo::vector4& vector, const holo::matrix& matrix);

		// Components of the vector.
		float x;
		float y;
		float z;
		float w;
	};
	
	holo::vector4 operator +(const holo::vector4& left, const holo::vector4& right);
	holo::vector4 operator +(const holo::vector4& left, float right);
	holo::vector4 operator +(float left, const holo::vector4& right);
	
	holo::vector4 operator -(const holo::vector4& left, const holo::vector4& right);
	holo::vector4 operator -(const holo::vector4& left, float right);
	holo::vector4 operator -(float left, const holo::vector4& right);
	
	holo::vector4 operator *(const holo::vector4& left, const holo::vector4& right);
	holo::vector4 operator *(const holo::vector4& left, float right);
	holo::vector4 operator *(float left, const holo::vector4& right);
	
	holo::vector4 operator /(const holo::vector4& left, const holo::vector4& right);
	holo::vector4 operator /(const holo::vector4& left, float right);
	holo::vector4 operator /(float left, const holo::vector4& right);
}

#endif
