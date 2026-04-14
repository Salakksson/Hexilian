#ifndef GRID_H_
#define GRID_H_

#include <stdexcept>
#include <format>
#include <unordered_map>
#include <vector>
#include <raylib.h>
#include <cmath>

#define CHUNK_SIZE 8

#define DRAW_CHUNK_BORDERS 0
#define CUTOFF_CORNERS 0
#define DEBUG_TEXT 1
#define TOUCH_ON_PLACE 1
#define DRAW_BG 1
#define DRAW_PIECES 1
#define OPTIMISE_OFFSCREEN_CHUNKS 0
#define SMALL_VIEWPORT 0

enum Piece
{
	PIECE_NONE = 0,
	PIECE_CROSS = 1,
	PIECE_CIRCLE = 2,
};

#define FLOOR_DIV(a, b) ((int)floor((double)(a) / (b)))
#define FLOOR_MOD(a, b) ((a) - (FLOOR_DIV((a), (b)) * (b)))

struct Coord
{
	int x = 0;
	int y = 0;

	constexpr bool operator==(const Coord& other) const
	{
		return x == other.x && y == other.y;
	}

	constexpr bool operator!=(const Coord& other) const
	{
		return x != other.x || y != other.y;
	}

	constexpr Coord (int x, int y): x(x), y(y) {}
	constexpr Coord () {}

	constexpr Coord chunk() const
	{
		return Coord (
			FLOOR_DIV(x, CHUNK_SIZE),
			FLOOR_DIV(y, CHUNK_SIZE)
		);
	}

	constexpr Coord tile() const
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

struct Chunk
{
private:
	Piece arr[CHUNK_SIZE * CHUNK_SIZE] = {PIECE_NONE};
public:

	bool empty = true;

	inline Piece get_at(Coord c)
	{
		return arr[c.x + c.y * CHUNK_SIZE];
	}

	inline void set_at(Coord c, Piece p)
	{
		if ( c.x < 0 || c.x >= CHUNK_SIZE ||
		     c.y < 0 || c.y >= CHUNK_SIZE )
		{
			printf(
				"attempted to set at invalid index %i %i\n",
				c.x, c.y
			);
			throw std::runtime_error("");
		}
		arr[c.x + c.y * CHUNK_SIZE] = p;

		empty = false;
	}

	inline void clear()
	{
		for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++)
			arr[i] = PIECE_NONE;
		empty = true;
	}

	inline Chunk()
	{
		clear();
	}
};

namespace std
{
	template <>
	struct hash<Coord>
	{
		size_t operator() (const Coord& c) const
		{
			return
				hash<int>()(c.x) ^
				hash<int>()(c.y) ;
		}
	};
}

struct Grid
{
	std::unordered_map<Coord, Chunk> chunks;

	inline Piece get_at(Coord coord)
	{
		return chunks[coord].get_at(coord.tile());
	}

	Piece turn = PIECE_CROSS;
	int n_turns = 1;

	int which_corner(Coord chunk);
	void touch_single_chunk(Coord chunk);
	void touch_chunk(Coord chunk);

	void place(Coord coord, Piece p);
	void play(Coord coord);
	void unplay(Coord coord);
	void draw();

};

Vector2 coord_center(Coord coord);
Coord vec_to_coord(Vector2 pos);

#endif
