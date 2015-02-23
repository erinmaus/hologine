// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_MATH_VECTOR3_H_
#define HOLOGINE_CORE_MATH_VECTOR3_H_

namespace holo
{
	struct matrix;
	
	// A three-dimensional vector.
	struct vector3
	{
		// Generates an empty vector.
		vector3();
		
		// Generates a vector from three scalars.
		vector3(float x, float y, float z);
		
		// Adds two vectors.
		holo::vector3& operator +=(const holo::vector3& other);
		
		// Adds a scalar to both components of a vector.
		holo::vector3& operator +=(float other);
		
		// Subtracts two vectors.
		holo::vector3& operator -=(const holo::vector3& other);
		
		// Subtracts a scalar from both components of a vector.
		holo::vector3& operator -=(float other);
		
		// Multiplies two vectors.
		holo::vector3& operator *=(const holo::vector3& other);
		
		// Multiplies both components of a vector by a scalar.
		holo::vector3& operator *=(float other);
		
		// Divides a vector by another.
		holo::vector3& operator /=(const holo::vector3& other);
		
		// Divides both components of a vector by a scalar.
		holo::vector3& operator /=(float other);
		
		// Negates a vector.
		holo::vector3 operator -() const;
		
		// Calculates the length squared of a vector.
		float calculate_length_squared() const;
		
		// Calculates the length of a vector.
		float calculate_length() const;
		
		// Returns the normal of the provided vector.
		static holo::vector3 normalize(const holo::vector3& vector);
		
		// Returns the cross product of the provided vectors.
		static holo::vector3 cross(const holo::vector3& left, const holo::vector3& right);
		
		// Returns the dot product of the provided vectors.
		static float dot(const holo::vector3& a, const holo::vector3& b);
		
		// Calculates the distance between two vectors.
		static float distance(const holo::vector3& from, const holo::vector3& to);
		
		// Transforms a vector by a matrix.
		static holo::vector3 transform(const holo::vector3& vector, const holo::matrix& matrix);
		
		// Transforms a normal by a matrix.
		//
		// Effectively, this ignores the translation component, and thus performs 
		// homogeneous transformation.
		static holo::vector3 transform_normal(const holo::vector3& vector, const holo::matrix& matrix);
	
		// Components of the vector
		float x;
		float y;
		float z;
	};
	
	holo::vector3 operator +(const holo::vector3& left, const holo::vector3& right);
	holo::vector3 operator +(const holo::vector3& left, float right);
	holo::vector3 operator +(float left, const holo::vector3& right);
	
	holo::vector3 operator -(const holo::vector3& left, const holo::vector3& right);
	holo::vector3 operator -(const holo::vector3& left, float right);
	holo::vector3 operator -(float left, const holo::vector3& right);
	
	holo::vector3 operator *(const holo::vector3& left, const holo::vector3& right);
	holo::vector3 operator *(const holo::vector3& left, float right);
	holo::vector3 operator *(float left, const holo::vector3& right);
	
	holo::vector3 operator /(const holo::vector3& left, const holo::vector3& right);
	holo::vector3 operator /(const holo::vector3& left, float right);
	holo::vector3 operator /(float left, const holo::vector3& right);
}

#endif
