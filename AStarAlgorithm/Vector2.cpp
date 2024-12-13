#include "Vector2.h"

Vector2::Vector2() {};
Vector2::Vector2(float x, float y) : x(x), y(y) {};
Vector2 Vector2::operator-(const Vector2& other) const
{
	return { x - other.x, y - other.y };
}

Vector2 Vector2::operator+(const Vector2& other) const
{
	return { x + other.x, y + other.y };
}

Vector2 Vector2::operator*(float scalar) const
{
	return { x * scalar, y * scalar };
}

float Vector2::Dot(const Vector2& other) const
{
	return x * other.x + y * other.y;
}

Vector2 Vector2::Normalize() const
{
	float length = std::sqrt(x * x + y * y);
	return { x / length, y / length };
}