#ifndef GRID_H_
#define GRID_H_

#include "coord.h"
#include "config.h"

#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <raylib.h>
#include <cmath>

enum class Piece
{
	none = 0,
	cross = 1,
	circle = 2,
};

struct Chunk
{
private:
	Piece arr[CHUNK_SIZE * CHUNK_SIZE] = {Piece::none};
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
			arr[i] = Piece::none;
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

struct Line
{
	int moveno_added;
	int moveno_removed;
	int length;
	Coord start;
	Coord end;
	Coord dir;
	// important:  start + dir * length = end
	// this means start is always left* of end
};

class Grid
{
	std::unordered_map<Coord, Chunk> chunks;
	std::vector<Line> old_lines;

	static constexpr Coord offset = Coord(CHUNK_SIZE / 2, CHUNK_SIZE / 2);

	int which_corner(Coord chunk);
	void touch_single_chunk(Coord chunk);
	void touch_chunk(Coord chunk);

	void draw_hex(Coord coord, Piece p);

	void add_lines(Coord center, Piece added);
	void remove_lines(Coord center, Piece removed);
	void update_lines(Coord center, Piece owner);

public:
	Piece turn = Piece::cross;
	int n_turns = 1;

	int moveno = 0;
	bool is_won = false;

	std::vector<Line> lines;
	std::vector<Coord> moves;

	inline Grid clone()
	{
		Grid grid;
		grid.chunks = this->chunks;
		grid.turn = this->turn;
		grid.n_turns = this->n_turns;
		grid.old_lines = this->old_lines;
		grid.moveno = this->moveno;
		grid.is_won = this->is_won;
		grid.lines = this->lines;
		return grid;
	}

	inline Piece get_at(Coord coord)
	{
		return chunks[coord.chunk()].get_at(coord.tile());
	}

	bool is_empty(Coord coord);
	bool is_equal(Coord coord, Piece piece);

	// void place(Coord coord, Piece p);
	void play(Coord coord);
	void unplay(Coord coord);
	void unplay();
	void draw();
};

Vector2 coord_center(Coord coord);
Coord vec_to_coord(Vector2 pos);

#endif
