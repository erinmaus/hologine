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
#include "core/math/vector4.hpp"

holo::vector4::vector4() :
	x(0.0f),
	y(0.0f),
	z(0.0f),
	w(0.0f)
{
	// Nothing.
}

holo::vector4::vector4(float x, float y, float z, float w) :
	x(x),
	y(y),
	z(z),
	w(w)
{
	// Nothing.
}

holo::vector4& holo::vector4::operator +=(const holo::vector4& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;
	
	return *this;
}

holo::vector4& holo::vector4::operator +=(float other)
{
	x += other;
	y += other;
	z += other;
	w += other;
	
	return *this;
}

holo::vector4& holo::vector4::operator -=(const holo::vector4& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;
	
	return *this;
}

holo::vector4& holo::vector4::operator -=(float other)
{
	x -= other;
	y -= other;
	z -= other;
	w -= other;
	
	return *this;
}

holo::vector4& holo::vector4::operator *=(const holo::vector4& other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;
	w *= other.w;
	
	return *this;
}

holo::vector4& holo::vector4::operator *=(float other)
{
	x *= other;
	y *= other;
	z *= other;
	w *= other;
	
	return *this;
}

holo::vector4& holo::vector4::operator /=(const holo::vector4& other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;
	w /= other.w;
	
	return *this;
}

holo::vector4& holo::vector4::operator /=(float other)
{
	x /= other;
	y /= other;
	z /= other;
	w /= other;
	
	return *this;
}

holo::vector4 holo::vector4::operator -() const
{
	return holo::vector4(-x, -y, -z, -w);
}

float holo::vector4::calculate_length_squared() const
{
	return x * x + y * y + z * z + w * w;
}

float holo::vector4::calculate_length() const
{
	return std::sqrt(calculate_length_squared());
}
	
holo::vector4 holo::vector4::normalize(const holo::vector4& vector)
{
	return vector * (1.0f / vector.calculate_length());
}

float holo::vector4::dot(const holo::vector4& a, const holo::vector4& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float holo::vector4::distance(const holo::vector4& from, const holo::vector4& to)
{
	return (from - to).calculate_length();
}

holo::vector4 holo::vector4::transform(const holo::vector4& vector, const holo::matrix& matrix)
{
	return holo::vector4
	(
		vector.x * matrix.m11 + vector.y * matrix.m12 + vector.z * matrix.m13 + vector.w * matrix.m14,
		vector.x * matrix.m21 + vector.y * matrix.m22 + vector.z * matrix.m23 + vector.w * matrix.m24,
		vector.x * matrix.m31 + vector.y * matrix.m32 + vector.z * matrix.m33 + vector.w * matrix.m34,
		vector.x * matrix.m41 + vector.y * matrix.m42 + vector.z * matrix.m43 + vector.w * matrix.m44
	);
}

holo::vector4 holo::operator +(const holo::vector4& left, const holo::vector4& right)
{
	return holo::vector4(left) += right;
}

holo::vector4 holo::operator +(const holo::vector4& left, float right)
{
	return holo::vector4(left) += right;
}

holo::vector4 holo::operator +(float left, const holo::vector4& right)
{
	return holo::vector4(left, left, left, left) += right;
}

holo::vector4 holo::operator -(const holo::vector4& left, const holo::vector4& right)
{
	return holo::vector4(left) -= right;
}

holo::vector4 holo::operator -(const holo::vector4& left, float right)
{
	return holo::vector4(left) -= right;
}

holo::vector4 holo::operator -(float left, const holo::vector4& right)
{
	return holo::vector4(left, left, left, left) -= right;
}

holo::vector4 holo::operator *(const holo::vector4& left, const holo::vector4& right)
{
	return holo::vector4(left) *= right;
}

holo::vector4 holo::operator *(const holo::vector4& left, float right)
{
	return holo::vector4(left) *= right;
}

holo::vector4 holo::operator *(float left, const holo::vector4& right)
{
	return holo::vector4(left, left, left, left) *= right;
}

holo::vector4 holo::operator /(const holo::vector4& left, const holo::vector4& right)
{
	return holo::vector4(left) /= right;
}

holo::vector4 holo::operator /(const holo::vector4& left, float right)
{
	return holo::vector4(left) /= right;
}

holo::vector4 holo::operator /(float left, const holo::vector4& right)
{
	return holo::vector4(left, left, left, left) /= right;
}
