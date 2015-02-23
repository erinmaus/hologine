// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#include <cmath>
#include "core/math/matrix.hpp"
#include "core/math/util.hpp"
#include "core/math/vector3.hpp"

holo::matrix::matrix() :
		m11(1.0f), m12(0.0f), m13(0.0f), m14(0.0f),
		m21(0.0f), m22(1.0f), m23(0.0f), m24(0.0f),
		m31(0.0f), m32(0.0f), m33(1.0f), m34(0.0f),
		m41(0.0f), 	m42(0.0f), m43(0.0f), m44(1.0f)
{
	// Nothing.
}

holo::matrix::matrix(const float other[16]) :
	m11(other[0]), m12(other[1]), m13(other[2]), m14(other[3]),
	m21(other[4]), m22(other[5]), m23(other[6]), m24(other[7]),
	m31(other[8]), m32(other[9]), m33(other[10]), m34(other[11]),
	m41(other[12]), m42(other[13]), m43(other[14]), m44(other[15])
{
	// Nothing.
}

holo::matrix::matrix(
	float m11, float m12, float m13, float m14,
	float m21, float m22, float m23, float m24,
	float m31, float m32, float m33, float m34,
	float m41, float m42, float m43, float m44) :
		m11(m11),	m12(m12),	m13(m13),	m14(m14),
		m21(m21),	m22(m22),	m23(m23),	m24(m24),
		m31(m31),	m32(m32),	m33(m33),	m34(m34),
		m41(m41),	m42(m42),	m43(m43),	m44(m44)
{
	// Nothing.
}

float* holo::matrix::get_pointer() const
{
	return (float*)&m11;
}

holo::matrix& holo::matrix::operator *=(const holo::matrix& other)
{
	matrix result;

	result.m11 = m11 * other.m11 + m12 * other.m21 + m13 * other.m31 + m14 * other.m41;
	result.m12 = m11 * other.m12 + m12 * other.m22 + m13 * other.m32 + m14 * other.m42;
	result.m13 = m11 * other.m13 + m12 * other.m23 + m13 * other.m33 + m14 * other.m43;
	result.m14 = m11 * other.m14 + m12 * other.m24 + m13 * other.m34 + m14 * other.m44;
	result.m21 = m21 * other.m11 + m22 * other.m21 + m23 * other.m31 + m24 * other.m41;
	result.m22 = m21 * other.m12 + m22 * other.m22 + m23 * other.m32 + m24 * other.m42;
	result.m23 = m21 * other.m13 + m22 * other.m23 + m23 * other.m33 + m24 * other.m43;
	result.m24 = m21 * other.m14 + m22 * other.m24 + m23 * other.m34 + m24 * other.m44;
	result.m31 = m31 * other.m11 + m32 * other.m21 + m33 * other.m31 + m34 * other.m41;
	result.m32 = m31 * other.m12 + m32 * other.m22 + m33 * other.m32 + m34 * other.m42;
	result.m33 = m31 * other.m13 + m32 * other.m23 + m33 * other.m33 + m34 * other.m43;
	result.m34 = m31 * other.m14 + m32 * other.m24 + m33 * other.m34 + m34 * other.m44;
	result.m41 = m41 * other.m11 + m42 * other.m21 + m43 * other.m31 + m44 * other.m41;
	result.m42 = m41 * other.m12 + m42 * other.m22 + m43 * other.m32 + m44 * other.m42;
	result.m43 = m41 * other.m13 + m42 * other.m23 + m43 * other.m33 + m44 * other.m43;
	result.m44 = m41 * other.m14 + m42 * other.m24 + m43 * other.m34 + m44 * other.m44;

	*this = result;
	
	return *this;
}

holo::matrix holo::matrix::rotate(const holo::vector3& axis, float angle)
{
	float c = std::cos(-angle);
	float s = std::sin(-angle);
	float t = 1.0 - c;
	
	return holo::matrix
	(
		t * axis.x * axis.x + c, t * axis.x * axis.y - s * axis.z, t * axis.x * axis.z + s * axis.y, 0.0f,
		t * axis.x * axis.y + s * axis.z, t * axis.y * axis.y + c, t * axis.y * axis.z - s * axis.x, 0.0f,
		t * axis.x * axis.z - s * axis.y, t * axis.y * axis.z + s * axis.x, t * axis.z * axis.z + c, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

holo::matrix holo::matrix::translate(const holo::vector3& translation)
{
	return holo::matrix
	(
		1.0f, 0.0f, 0.0f, translation.x,
		0.0f, 1.0f, 0.0f, translation.y,
		0.0f, 0.0f, 1.0f, translation.z,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

holo::matrix holo::matrix::scale(const holo::vector3& scale)
{
	return holo::matrix
	(
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

float holo::matrix::calculate_determinant() const
{
	float d1 = (m33 * m44) - (m34 * m43);
	float d2 = (m32 * m44) - (m34 * m42);
	float d3 = (m32 * m43) - (m33 * m42);
	float d4 = (m31 * m44) - (m34 * m41);
	float d5 = (m31 * m43) - (m33 * m41);
	float d6 = (m31 * m42) - (m32 * m41);

	return ((((m11 * (((m22 * d1) - (m23 * d2)) + (m24 * d3))) - (m12 * (((m21 * d1) -
		(m23 * d4)) + (m24 * d5)))) + (m13 * (((m21 * d2) - (m22 * d4)) +
		(m24 * d6)))) - (m14 * (((m21 * d3) - (m22 * d5)) + (m23 * d6))));
}

holo::matrix holo::matrix::invert(const holo::matrix& matrix)
{
	float d = 1.0f / matrix.calculate_determinant();
	holo::matrix result;

	result.m11 = (matrix.m22 * matrix.m33 * matrix.m44 + matrix.m23 * matrix.m34 * matrix.m42 + matrix.m24 * matrix.m32 * matrix.m43 - matrix.m22 * matrix.m34 * matrix.m43 - matrix.m23 * matrix.m32 * matrix.m44 - matrix.m24 * matrix.m33 * matrix.m42) * d;
	result.m12 = (matrix.m12 * matrix.m34 * matrix.m43 + matrix.m13 * matrix.m32 * matrix.m44 + matrix.m14 * matrix.m33 * matrix.m42 - matrix.m12 * matrix.m33 * matrix.m44 - matrix.m13 * matrix.m34 * matrix.m42 - matrix.m14 * matrix.m32 * matrix.m43) * d;
	result.m13 = (matrix.m12 * matrix.m23 * matrix.m44 + matrix.m13 * matrix.m24 * matrix.m42 + matrix.m14 * matrix.m22 * matrix.m43 - matrix.m12 * matrix.m24 * matrix.m43 - matrix.m13 * matrix.m22 * matrix.m44 - matrix.m14 * matrix.m23 * matrix.m42) * d;
	result.m14 = (matrix.m12 * matrix.m24 * matrix.m33 + matrix.m13 * matrix.m22 * matrix.m34 + matrix.m14 * matrix.m23 * matrix.m32 - matrix.m12 * matrix.m23 * matrix.m34 - matrix.m13 * matrix.m24 * matrix.m32 - matrix.m14 * matrix.m22 * matrix.m33) * d;
	result.m21 = (matrix.m21 * matrix.m34 * matrix.m43 + matrix.m23 * matrix.m31 * matrix.m44 + matrix.m24 * matrix.m33 * matrix.m41 - matrix.m21 * matrix.m33 * matrix.m44 - matrix.m23 * matrix.m34 * matrix.m41 - matrix.m24 * matrix.m31 * matrix.m43) * d;
	result.m22 = (matrix.m11 * matrix.m33 * matrix.m44 + matrix.m13 * matrix.m34 * matrix.m41 + matrix.m14 * matrix.m31 * matrix.m43 - matrix.m11 * matrix.m34 * matrix.m43 - matrix.m13 * matrix.m31 * matrix.m44 - matrix.m14 * matrix.m33 * matrix.m41) * d;
	result.m23 = (matrix.m11 * matrix.m24 * matrix.m43 + matrix.m13 * matrix.m21 * matrix.m44 + matrix.m14 * matrix.m23 * matrix.m41 - matrix.m11 * matrix.m23 * matrix.m44 - matrix.m13 * matrix.m24 * matrix.m41 - matrix.m14 * matrix.m21 * matrix.m43) * d;
	result.m24 = (matrix.m11 * matrix.m23 * matrix.m34 + matrix.m13 * matrix.m24 * matrix.m31 + matrix.m14 * matrix.m21 * matrix.m33 - matrix.m11 * matrix.m24 * matrix.m33 - matrix.m13 * matrix.m21 * matrix.m34 - matrix.m14 * matrix.m23 * matrix.m31) * d;
	result.m31 = (matrix.m21 * matrix.m32 * matrix.m44 + matrix.m22 * matrix.m34 * matrix.m41 + matrix.m24 * matrix.m31 * matrix.m42 - matrix.m21 * matrix.m34 * matrix.m42 - matrix.m22 * matrix.m31 * matrix.m44 - matrix.m24 * matrix.m32 * matrix.m41) * d;
	result.m32 = (matrix.m11 * matrix.m34 * matrix.m42 + matrix.m12 * matrix.m31 * matrix.m44 + matrix.m14 * matrix.m32 * matrix.m41 - matrix.m11 * matrix.m32 * matrix.m44 - matrix.m12 * matrix.m34 * matrix.m41 - matrix.m14 * matrix.m31 * matrix.m42) * d;
	result.m33 = (matrix.m11 * matrix.m22 * matrix.m44 + matrix.m12 * matrix.m24 * matrix.m41 + matrix.m14 * matrix.m21 * matrix.m42 - matrix.m11 * matrix.m24 * matrix.m42 - matrix.m12 * matrix.m21 * matrix.m44 - matrix.m14 * matrix.m22 * matrix.m41) * d;
	result.m34 = (matrix.m11 * matrix.m24 * matrix.m32 + matrix.m12 * matrix.m21 * matrix.m34 + matrix.m14 * matrix.m22 * matrix.m31 - matrix.m11 * matrix.m22 * matrix.m34 - matrix.m12 * matrix.m24 * matrix.m31 - matrix.m14 * matrix.m21 * matrix.m32) * d;
	result.m41 = (matrix.m21 * matrix.m33 * matrix.m42 + matrix.m22 * matrix.m31 * matrix.m43 + matrix.m23 * matrix.m32 * matrix.m41 - matrix.m21 * matrix.m32 * matrix.m43 - matrix.m22 * matrix.m33 * matrix.m41 - matrix.m23 * matrix.m31 * matrix.m42) * d;
	result.m42 = (matrix.m11 * matrix.m32 * matrix.m43 + matrix.m12 * matrix.m33 * matrix.m41 + matrix.m13 * matrix.m31 * matrix.m42 - matrix.m11 * matrix.m33 * matrix.m42 - matrix.m12 * matrix.m31 * matrix.m43 - matrix.m13 * matrix.m32 * matrix.m41) * d;
	result.m43 = (matrix.m11 * matrix.m23 * matrix.m42 + matrix.m12 * matrix.m21 * matrix.m43 + matrix.m13 * matrix.m22 * matrix.m41 - matrix.m11 * matrix.m22 * matrix.m43 - matrix.m12 * matrix.m23 * matrix.m41 - matrix.m13 * matrix.m21 * matrix.m42) * d;
	result.m44 = (matrix.m11 * matrix.m22 * matrix.m33 + matrix.m12 * matrix.m23 * matrix.m31 + matrix.m13 * matrix.m21 * matrix.m32 - matrix.m11 * matrix.m23 * matrix.m32 - matrix.m12 * matrix.m21 * matrix.m33 - matrix.m13 * matrix.m22 * matrix.m31) * d;
	
	return result;
}

holo::matrix holo::matrix::transpose(const holo::matrix& matrix)
{
	return holo::matrix
	(
		matrix.m11, matrix.m21, matrix.m31, matrix.m41,
		matrix.m12, matrix.m22, matrix.m32, matrix.m42,
		matrix.m13, matrix.m23, matrix.m33, matrix.m43,
		matrix.m14, matrix.m24, matrix.m34, matrix.m44
	);
}

holo::matrix holo::matrix::ortho(float left, float right, float bottom, float top, float near, float far)
{
	float rightLeft = 1.0f / (right - left);
	float topBottom = 1.0f / (top - bottom);
	float farNear = 1.0f / (far - near);
	
	return holo::matrix
	(
		2.0f * rightLeft, 0.0f, 0.0f, -(right + left) * rightLeft,
		0.0f, 2.0f * topBottom, 0.0f, -(top + bottom) * topBottom, 
		0.0f, 0.0f, -farNear, -(far) * farNear,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

holo::matrix holo::matrix::perspective(float fov, float aspect, float near, float far)
{
	float f = std::tan(math::pi / 2.0f - fov / 2.0f);
	float a = f / aspect;
	float b = (far + near) / (near - far);
	float c = (2.0f * far * near) / (near - far);

	return holo::matrix
	(
		a, 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, b, c,
		0.0f, 0.0f, -1.0f, 0.0f
	);
}

holo::matrix holo::matrix::frustum(float left, float right, float bottom, float top, float near, float far)
{
	float x = (2.0f * near) / (right - left);
	float y = (2.0f * near) / (top - bottom);
	float a = (right + left) / (right - left);
	float b = (top + bottom) / (top - bottom);
	float c = -(near) / (far - near);
	float d = -(near * far) / (far - near);

	return holo::matrix
	(
		x, 0.0f, a, 0.0f,
		0.0f, y, b, 0.0f,
		0.0f, 0.0f, c, d,
		0.0f, 0.0f, -1.0f, 0.0f
	);
}

holo::matrix holo::matrix::look_at(const holo::vector3& eye, const holo::vector3& target, const holo::vector3& up)
{
	holo::vector3 f = vector3::normalize(target - eye);
	holo::vector3 s = vector3::normalize(vector3::cross(f, up));
	holo::vector3 u = vector3::normalize(vector3::cross(s, f));

	holo::matrix m
	(
		s.x, s.y, s.z, 0.0f,
		u.x, u.y, u.z, 0.0f,
		-f.x, -f.y, -f.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	return m * matrix::translate(-eye);
}

holo::matrix holo::operator *(const holo::matrix& left, const holo::matrix& right)
{
	return holo::matrix(left) *= right;
}
