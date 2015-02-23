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
#include "core/math/vector2.hpp"

holo::vector2::vector2() :
	x(0.0f),
	y(0.0f)
{
	// Nothing.
}

holo::vector2::vector2(float x, float y) :
	x(x),
	y(y)
{
	// Nothing.
}

holo::vector2& holo::vector2::operator +=(const holo::vector2& other)
{
	x += other.x;
	y += other.y;
	
	return *this;
}

holo::vector2& holo::vector2::operator +=(float other)
{
	x += other;
	y += other;
	
	return *this;
}

holo::vector2& holo::vector2::operator -=(const holo::vector2& other)
{
	x -= other.x;
	y -= other.y;
	
	return *this;
}

holo::vector2& holo::vector2::operator -=(float other)
{
	x -= other;
	y -= other;
	
	return *this;
}

holo::vector2& holo::vector2::operator *=(const holo::vector2& other)
{
	x *= other.x;
	y *= other.y;
	
	return *this;
}

holo::vector2& holo::vector2::operator *=(float other)
{
	x *= other;
	y *= other;
	
	return *this;
}

holo::vector2& holo::vector2::operator /=(const holo::vector2& other)
{
	x /= other.x;
	y /= other.y;
	
	return *this;
}

holo::vector2& holo::vector2::operator /=(float other)
{
	x /= other;
	y /= other;
	
	return *this;
}

holo::vector2 holo::vector2::operator -() const
{
	return holo::vector2(-x, -y);
}

float holo::vector2::calculate_length_squared() const
{
	return x * x + y * y;
}

float holo::vector2::calculate_length() const
{
	return std::sqrt(calculate_length_squared());
}
	
holo::vector2 holo::vector2::normalize(const holo::vector2& vector)
{
	return vector * (1.0f / vector.calculate_length());
}

float holo::vector2::dot(const holo::vector2& a, const holo::vector2& b)
{
	return a.x * b.x + a.y * b.y;
}

float holo::vector2::distance(const holo::vector2& from, const holo::vector2& to)
{
	return (from - to).calculate_length();
}

holo::vector2 holo::operator +(const holo::vector2& left, const holo::vector2& right)
{
	return holo::vector2(left) += right;
}

holo::vector2 holo::operator +(const holo::vector2& left, float right)
{
	return holo::vector2(left) += right;
}

holo::vector2 holo::operator +(float left, const holo::vector2& right)
{
	return holo::vector2(left, left) += right;
}

holo::vector2 holo::operator -(const holo::vector2& left, const holo::vector2& right)
{
	return holo::vector2(left) -= right;
}

holo::vector2 holo::operator -(const holo::vector2& left, float right)
{
	return holo::vector2(left) -= right;
}

holo::vector2 holo::operator -(float left, const holo::vector2& right)
{
	return holo::vector2(left, left) -= right;
}

holo::vector2 holo::operator *(const holo::vector2& left, const holo::vector2& right)
{
	return holo::vector2(left) *= right;
}

holo::vector2 holo::operator *(const holo::vector2& left, float right)
{
	return holo::vector2(left) *= right;
}

holo::vector2 holo::operator *(float left, const holo::vector2& right)
{
	return holo::vector2(left, left) *= right;
}

holo::vector2 holo::operator /(const holo::vector2& left, const holo::vector2& right)
{
	return holo::vector2(left) /= right;
}

holo::vector2 holo::operator /(const holo::vector2& left, float right)
{
	return holo::vector2(left) /= right;
}

holo::vector2 holo::operator /(float left, const holo::vector2& right)
{
	return holo::vector2(left, left) /= right;
}
