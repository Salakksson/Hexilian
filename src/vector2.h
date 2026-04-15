#ifndef OPERATORS_H_
#define OPERATORS_H_

#include <raylib.h>

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

inline constexpr Vector2 operator*(Vector2 vect, float f) {
	return Vector2 {
		.x = vect.x * f,
		.y = vect.y * f
	};
}

inline constexpr Vector2 operator/(Vector2 vect, float f) {
	return Vector2 {
		.x = vect.x / f,
		.y = vect.y / f
	};
}

inline constexpr Vector2 operator+(Vector2 a, Vector2 b) {
	return Vector2 {
		.x = a.x + b.x,
		.y = a.y + b.y
	};
}

inline constexpr Vector2 operator-(Vector2 a, Vector2 b) {
	return Vector2 {
		.x = a.x - b.x,
		.y = a.y - b.y
	};
}

inline constexpr Vector2 operator*=(Vector2& vect, float f) {
	return vect = vect * f;
}

inline constexpr Vector2 operator/=(Vector2& vect, float f) {
	return vect = vect / f;
}

inline constexpr Vector2 operator+=(Vector2& a, Vector2 b) {
	return a = a + b;
}

inline constexpr Vector2 operator-=(Vector2& a, Vector2 b) {
	return a = a - b;
}

inline constexpr Vector2 operator*(Vector2 a, Vector2 b) {
	return Vector2 {
		.x = a.x * b.x,
		.y = a.y * b.y
	};
}

inline constexpr Vector2 operator/(Vector2 a, Vector2 b) {
	return Vector2 {
		.x = a.x / b.x,
		.y = a.y / b.y
	};
}

inline constexpr Vector2 operator*=(Vector2& a, Vector2 b) {
	return a = a * b;
}

inline constexpr Vector2 operator/=(Vector2& a, Vector2 b) {
	return a = a / b;
}

#endif
