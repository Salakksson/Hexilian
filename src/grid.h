#ifndef GRID_H_
#define GRID_H_

#include "coord.h"
#include "config.h"

#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <raylib.h>
#include <cmath>

enum Piece
{
	PIECE_NONE = 0,
	PIECE_CROSS = 1,
	PIECE_CIRCLE = 2,
};

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
