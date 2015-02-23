// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_MATH_MATRIX_H_
#define HOLOGINE_CORE_MATH_MATRIX_H_

namespace holo
{
	struct vector3;
	
	// A four-dimensional matrix, stored in row-major order.
	//
	// Four-dimensional matrices are used for transformations and projections
	// by graphics and physics APIs, among others.
	struct matrix
	{
		// Generates an identity matrix.
		matrix();
		
		// Generates a matrix from a 16-component array.
		matrix(const float other[16]);
		
		// Generates a matrix from arguments.
		matrix(
			float m11, float m12, float m13, float m14,
			float m21, float m22, float m23, float m24,
			float m31, float m32, float m33, float m34,
			float m41, float m42, float m43, float m44);

		// Converts the matrix to a suitable representaiton for other APIs.
		//
		// This is equivalent of &m.m11.
		float* get_pointer() const;
		
		// Multiplies this matrix by another.
		holo::matrix& operator *=(const holo::matrix& other);
		
		// Calculates the determinant of the matrix.
		float calculate_determinant() const;
		
		// Generates a rotation matrix around the provided axis and angle,
		// in radians.
		static holo::matrix rotate(const holo::vector3& axis, float angle);
		
		// Generates a translation matrix, representing the provided
		// transformation.
		static holo::matrix translate(const holo::vector3& translation);
		
		// Generates a scale matrix, representing the provided
		// transformation.
		static holo::matrix scale(const holo::vector3& scale);
		
		// Returns a matrix that is the inverse of the provided matrix.
		static holo::matrix invert(const holo::matrix& matrix);
		
		// Returns a matrix that is the transpose of the provided matrix.
		static holo::matrix transpose(const holo::matrix& input);
		
		// Generates an orthographic projection matrix with the provided
		// arguments.
		static holo::matrix ortho(
			float left,
			float right,
			float bottom,
			float top,
			float near,
			float far);
		
		// Generates a perspective projection matrix with the provided
		// arguments.
		static holo::matrix perspective(
			float fov,
			float aspect,
			float near,
			float far);
		
		// Generates a perspective projection matrix with the provided
		// arguments.
		static holo::matrix frustum(
			float left,
			float right,
			float bottom,
			float top,
			float near,
			float far);
		
		// Generates a camera (look-at) matrix from the provided arguments.
		static holo::matrix look_at(
			const holo::vector3& eye,
			const holo::vector3& target,
			const holo::vector3& up);

		// Components of the matrix.
		float m11, m12, m13, m14;
		float m21, m22, m23, m24;
		float m31, m32, m33, m34;
		float m41, m42, m43, m44;
	};
	
	// Multiplies one matrix by another.
	holo::matrix operator *(const holo::matrix& left, const holo::matrix& right);
}

#endif
