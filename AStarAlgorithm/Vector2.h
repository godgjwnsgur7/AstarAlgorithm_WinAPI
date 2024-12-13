#pragma once

#include "framework.h"

struct Vector2
{
	float x, y;
	Vector2();
	Vector2(float x, float y);
	Vector2 operator-(const Vector2& other) const;
	Vector2 operator+(const Vector2& other) const;
	Vector2 operator*(float scalar) const;
	float Dot(const Vector2& other) const;
	Vector2 Normalize() const;
};