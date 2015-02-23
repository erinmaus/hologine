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
#include "core/math/vector3.hpp"

holo::vector3::vector3() :
	x(0.0f),
	y(0.0f),
	z(0.0f)
{
	// Nothing.
}

holo::vector3::vector3(float x, float y, float z) :
	x(x),
	y(y),
	z(z)
{
	// Nothing.
}

holo::vector3& holo::vector3::operator +=(const holo::vector3& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	
	return *this;
}

holo::vector3& holo::vector3::operator +=(float other)
{
	x += other;
	y += other;
	z += other;
	
	return *this;
}

holo::vector3& holo::vector3::operator -=(const holo::vector3& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	
	return *this;
}

holo::vector3& holo::vector3::operator -=(float other)
{
	x -= other;
	y -= other;
	z -= other;
	
	return *this;
}

holo::vector3& holo::vector3::operator *=(const holo::vector3& other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;
	
	return *this;
}

holo::vector3& holo::vector3::operator *=(float other)
{
	x *= other;
	y *= other;
	z *= other;
	
	return *this;
}

holo::vector3& holo::vector3::operator /=(const holo::vector3& other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;
	
	return *this;
}

holo::vector3& holo::vector3::operator /=(float other)
{
	x /= other;
	y /= other;
	z /= other;
	
	return *this;
}

holo::vector3 holo::vector3::operator -() const
{
	return holo::vector3(-x, -y, -z);
}

float holo::vector3::calculate_length_squared() const
{
	return x * x + y * y + z * z;
}

float holo::vector3::calculate_length() const
{
	return std::sqrt(calculate_length_squared());
}
	
holo::vector3 holo::vector3::normalize(const holo::vector3& vector)
{
	return vector * (1.0f / vector.calculate_length());
}

holo::vector3 holo::vector3::cross(const holo::vector3& left, const holo::vector3& right)
{
	return holo::vector3(left.y * right.z - left.z * right.y, left.z * right.x - left.x * right.z, left.x * right.y - left.y * right.x);
}

float holo::vector3::dot(const holo::vector3& a, const holo::vector3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float holo::vector3::distance(const holo::vector3& from, const holo::vector3& to)
{
	return (from - to).calculate_length();
}

holo::vector3 holo::vector3::transform(const holo::vector3& vector, const holo::matrix& matrix)
{
	return holo::vector3
	(
		vector.x * matrix.m11 + vector.y * matrix.m12 + vector.z * matrix.m13 + matrix.m14,
		vector.x * matrix.m21 + vector.y * matrix.m22 + vector.z * matrix.m23 + matrix.m24,
		vector.x * matrix.m31 + vector.y * matrix.m32 + vector.z * matrix.m33 + matrix.m34
	);
}

holo::vector3 holo::vector3::transform_normal(const holo::vector3& vector, const holo::matrix& matrix)
{
	return holo::vector3
	(
		vector.x * matrix.m11 + vector.y * matrix.m12 + vector.z * matrix.m13,
		vector.x * matrix.m21 + vector.y * matrix.m22 + vector.z * matrix.m23,
		vector.x * matrix.m31 + vector.y * matrix.m32 + vector.z * matrix.m33
	);
}

holo::vector3 holo::operator +(const holo::vector3& left, const holo::vector3& right)
{
	return holo::vector3(left) += right;
}

holo::vector3 holo::operator +(const holo::vector3& left, float right)
{
	return holo::vector3(left) += right;
}

holo::vector3 holo::operator +(float left, const holo::vector3& right)
{
	return holo::vector3(left, left, left) += right;
}

holo::vector3 holo::operator -(const holo::vector3& left, const holo::vector3& right)
{
	return holo::vector3(left) -= right;
}

holo::vector3 holo::operator -(const holo::vector3& left, float right)
{
	return holo::vector3(left) -= right;
}

holo::vector3 holo::operator -(float left, const holo::vector3& right)
{
	return holo::vector3(left, left, left) -= right;
}

holo::vector3 holo::operator *(const holo::vector3& left, const holo::vector3& right)
{
	return holo::vector3(left) *= right;
}

holo::vector3 holo::operator *(const holo::vector3& left, float right)
{
	return holo::vector3(left) *= right;
}

holo::vector3 holo::operator *(float left, const holo::vector3& right)
{
	return holo::vector3(left, left, left) *= right;
}

holo::vector3 holo::operator /(const holo::vector3& left, const holo::vector3& right)
{
	return holo::vector3(left) /= right;
}

holo::vector3 holo::operator /(const holo::vector3& left, float right)
{
	return holo::vector3(left) /= right;
}

holo::vector3 holo::operator /(float left, const holo::vector3& right)
{
	return holo::vector3(left, left, left) /= right;
}
