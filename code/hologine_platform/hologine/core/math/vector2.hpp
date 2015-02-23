// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_MATH_VECTOR2_H_
#define HOLOGINE_CORE_MATH_VECTOR2_H_

namespace holo
{
	// A two-dimensional vector.
	struct vector2
	{
		// Generates an empty vector.
		vector2();
		
		// Generates a vector from two scalars.
		vector2(float x, float y);
		
		// Adds two vectors.
		holo::vector2& operator +=(const holo::vector2& other);
		
		// Adds a scalar to the components of a vector.
		holo::vector2& operator +=(float other);
		
		// Subtracts two vectors.
		holo::vector2& operator -=(const holo::vector2& other);
		
		// Subtracts a scalar from the components of a vector.
		holo::vector2& operator -=(float other);
		
		// Multiplies two vectors.
		holo::vector2& operator *=(const holo::vector2& other);
		
		// Multiplies the components of a vector by a scalar.
		holo::vector2& operator *=(float other);
		
		// Divides a vector by another.
		holo::vector2& operator /=(const holo::vector2& other);
		
		// Divides the components of a vector by a scalar.
		holo::vector2& operator /=(float other);
		
		// Negates a vector.
		holo::vector2 operator -() const;
		
		// Calculates the length squared of a vector.
		float calculate_length_squared() const;
		
		// Calculates the length of a vector.
		float calculate_length() const;
		
		// Returns the normal of the provided vector.
		static holo::vector2 normalize(const holo::vector2& vector);
		
		// Returns the dot product of the provided vectors.
		static float dot(const holo::vector2& a, const holo::vector2& b);
		
		// Calculates the distance between two vectors.
		static float distance(const holo::vector2& from, const holo::vector2& to);
	
		// Components of the vector
		float x;
		float y;
	};
	
	holo::vector2 operator +(const holo::vector2& left, const holo::vector2& right);
	holo::vector2 operator +(const holo::vector2& left, float right);
	holo::vector2 operator +(float left, const holo::vector2& right);
	
	holo::vector2 operator -(const holo::vector2& left, const holo::vector2& right);
	holo::vector2 operator -(const holo::vector2& left, float right);
	holo::vector2 operator -(float left, const holo::vector2& right);
	
	holo::vector2 operator *(const holo::vector2& left, const holo::vector2& right);
	holo::vector2 operator *(const holo::vector2& left, float right);
	holo::vector2 operator *(float left, const holo::vector2& right);
	
	holo::vector2 operator /(const holo::vector2& left, const holo::vector2& right);
	holo::vector2 operator /(const holo::vector2& left, float right);
	holo::vector2 operator /(float left, const holo::vector2& right);
}

#endif
