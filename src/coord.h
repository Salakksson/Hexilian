#ifndef COORD_H_
#define COORD_H_

#include "config.h"

#include <cmath>

// TODO: make these optimised and constexpr (assembly?)
#define FLOOR_DIV(a, b) ((int)floor((double)(a) / (b)))
#define FLOOR_MOD(a, b) ((a) - (FLOOR_DIV((a), (b)) * (b)))

struct Coord
{
	int x = 0;
	int y = 0;

	constexpr inline bool operator==(const Coord& other) const
	{
		return x == other.x && y == other.y;
	}

	constexpr inline bool operator!=(const Coord& other) const
	{
		return x != other.x || y != other.y;
	}

	constexpr inline Coord (int x, int y): x(x), y(y) {}
	constexpr inline Coord () {}

	inline Coord chunk() const
	{
		return Coord (
			FLOOR_DIV(x, CHUNK_SIZE),
			FLOOR_DIV(y, CHUNK_SIZE)
		);
	}

	inline Coord tile() const
	{
		return Coord (
			FLOOR_MOD(x, CHUNK_SIZE),
			FLOOR_MOD(y, CHUNK_SIZE)
		);
	}
};

constexpr inline Coord operator*(Coord vect, float f) {
	return Coord (vect.x * f, vect.y * f );
}

constexpr inline Coord operator/(Coord vect, float f) {
	return Coord (vect.x / f, vect.y / f );
}

constexpr inline Coord operator+(Coord a, Coord b) {
	return Coord (a.x + b.x, a.y + b.y );
}

constexpr inline Coord operator-(Coord a, Coord b) {
	return Coord (a.x - b.x, a.y - b.y );
}

constexpr inline Coord operator*=(Coord& coord, float f) {
	return coord = coord * f;
}

constexpr inline Coord operator/=(Coord& coord, float f) {
	return coord = coord / f;
}

constexpr inline Coord operator+=(Coord& a, Coord b) {
	return a = a + b;
}

constexpr inline Coord operator-=(Coord& a, Coord b) {
	return a = a - b;
}

constexpr inline Coord operator*(Coord a, Coord b) {
	return Coord ( a.x * b.x, a.y * b.y);
}

constexpr inline Coord operator/(Coord a, Coord b) {
	return Coord (a.x / b.x, a.y / b.y);
}

constexpr inline Coord operator*=(Coord& a, Coord b) {
	return a = a * b;
}

constexpr inline Coord operator/=(Coord& a, Coord b) {
	return a = a / b;
}

constexpr inline Coord world_coord(Coord chunk, Coord tile)
{
	return Coord (
		chunk.x * CHUNK_SIZE + tile.x,
		chunk.y * CHUNK_SIZE + tile.y
	);
}

#endif
