#include "grid.h"

#include <raylib.h>
#include <format>

#include "renderer.h"
#include "operators.h"

#define SQRT3 1.7320508075f

Vector2 coord_center(Coord coord)
{
	Vector2 pos = {
		(float)coord.x + (float)coord.y / 2,
		- (float)coord.y * SQRT3 / 2
	};

	return pos;
}

Coord vec_to_coord(Vector2 pos)
{
	pos.y = -pos.y;

	float q = pos.x - pos.y / sqrt(3.0f);
	float r = pos.y * 2.0f / sqrt(3.0f);

	// cube coords
	float x = q;
	float z = r;
	float y = -x - z;

	int rx = roundf(x);
	int ry = roundf(y);
	int rz = roundf(z);

	float dx = fabs(rx - x);
	float dy = fabs(ry - y);
	float dz = fabs(rz - z);

	if (dx > dy && dx > dz)
		rx = -ry - rz;
	else if (dy > dz)
		ry = -rx - rz;
	else
		rz = -rx - ry;

	return Coord(rx, rz);
}

Color piece_color(Piece p)
{
	switch (p)
	{
	case PIECE_NONE: return GetColor(0x1a1b26ff);
	case PIECE_CROSS: return GetColor(0xf7768eff);
	case PIECE_CIRCLE: return GetColor(0x0db9d7ff);
	default: return WHITE;
	}
}

void draw_empty_grid()
{
	const Vector2 top_left = {
		rend.world_viewport.x,
		rend.world_viewport.y
	};

	const Vector2 bottom_right = {
		rend.world_viewport.x + rend.world_viewport.width,
		rend.world_viewport.y + rend.world_viewport.height
	};

	const Coord top_left_coord = vec_to_coord(top_left);
	const Coord bottom_right_coord = vec_to_coord(bottom_right);

	int min_x = top_left_coord.x;
	int max_x = bottom_right_coord.x;
	int min_y = bottom_right_coord.y;
	int max_y = top_left_coord.y;

	int size_x = max_x - min_x;
	int size_y = max_y - min_y;

	int count = 0;
	for (int x = 0; x <= size_x; x++)
	{
		for (int y = 0; y <= size_y; y++)
		{
			if (2 * x + y > 2 * size_x + 1) continue;
			if (2 * x + y < size_x / 2 - 2) continue;

			count++;

			Coord coord = Coord(min_x + x, min_y + y);
			Vector2 pos = coord_center(coord);

			float width = 0.03;

			DrawPolyLinesEx(
				pos,
				6,
				0.5,
				30,
				width,
				GetColor(0xc0caf5ff)
			);
		}
	}
#if DEBUG_TEXT
	rend.end_cam();
	DrawText(std::format("Hexagons drawn: {}", count).c_str(), 5, 45, 20, GetColor(0xffffffff));
	rend.begin_cam();
#endif
}


void draw_hex(Coord coord, Piece p)
{
	if (p == PIECE_NONE) return;
	Vector2 pos = coord_center(coord);

	// full is SQRT3/2
	DrawPoly(pos, 6, 0.5, 30, piece_color(p));
}

void Grid::touch_single_chunk(Coord chunk)
{
	chunks.try_emplace(chunk);
}

void Grid::touch_chunk(Coord chunk)
{
	for (int dx = -1; dx <= 1; dx++)
		for (int dy = -1; dy <= 1; dy++)
		{
			Coord neighbour = Coord(
				chunk.x + dx,
				chunk.y + dy
			);
			touch_single_chunk(neighbour);
		}
}

void Grid::place(Coord coord, Piece p)
{
#if TOUCH_ON_PLACE
	touch_chunk(coord.chunk());
#endif
	chunks[coord.chunk()].set_at(coord.tile(), p);
}

void Grid::play(Coord coord)
{
	// tile already taken
	if (chunks[coord.chunk()].get_at(coord.tile()) != PIECE_NONE)
		return;

#if TOUCH_ON_PLACE
	touch_chunk(coord.chunk());
#endif
	chunks[coord.chunk()].set_at(coord.tile(), turn);

	n_turns--;
	if (n_turns == 0)
	{
		n_turns = 2;
		switch (turn)
		{
		case PIECE_CROSS: turn = PIECE_CIRCLE; break;
		case PIECE_CIRCLE: turn = PIECE_CROSS; break;
		default: turn = PIECE_NONE; break;
		}
	}
}

void Grid::unplay(Coord coord)
{
	// tile empty
	if (chunks[coord.chunk()].get_at(coord.tile()) == PIECE_NONE)
		return;

	chunks[coord.chunk()].set_at(coord.tile(), PIECE_NONE);

	if (n_turns == 2)
	{
		n_turns = 0;
		switch (turn)
		{
		case PIECE_CROSS: turn = PIECE_CIRCLE; break;
		case PIECE_CIRCLE: turn = PIECE_CROSS; break;
		default: turn = PIECE_NONE; break;
		}
	}

	n_turns++;
}

Coord hex_directions[6] = {
	{1, 0},
	{0, 1},
	{-1, 1},
	{-1, 0},
	{0, -1},
	{1, -1},
};

int Grid::which_corner(Coord chunk)
{
#if !CUTOFF_CORNERS
	return 0;
#endif
	bool north = (chunks.contains(chunk + Coord(0, 1)));
	bool east = (chunks.contains(chunk + Coord(1, 0)));

	if (!north && !east) {
		return 1;
	}

	bool south = (chunks.contains(chunk + Coord(0, -1)));
	bool west = (chunks.contains(chunk + Coord(-1, 0)));

	if (!south && !west) {
		return 2;
	}

	return 0;
}

inline Rectangle chunk_border(Coord chunk)
{
	constexpr Vector2 chunk_size = {
		-0.5 + 1.5 * (float)CHUNK_SIZE,
		(SQRT3/6) + SQRT3/2 * (float)CHUNK_SIZE,
	};

	Coord chunk_corner_coord = chunk * CHUNK_SIZE;

	Vector2 chunk_corner = {
		coord_center(chunk_corner_coord).x - 0.5f,
		coord_center(chunk_corner_coord).y + SQRT3/3
		- chunk_size.y,
	};

	Rectangle border = {
		chunk_corner.x,
		chunk_corner.y,
		chunk_size.x,
		chunk_size.y,
	};

	return border;
}

inline bool is_chunk_on_screen(Coord chunk)
{
#if OPTIMISE_OFFSCREEN_CHUNKS
	return CheckCollisionRecs (
		chunk_border(chunk),
		rend.world_viewport
	);
#else
	return true;
#endif
}

void Grid::draw()
{
	Vector2 mouse = rend.mouse_pos_world();
	Coord m = vec_to_coord(mouse);

	int count = 0;

#if DRAW_PIECES

	for (auto i : chunks)
	{
		Coord pos = i.first;
		Chunk chunk = i.second;

		if (!is_chunk_on_screen(pos)) continue;
		count++;

		int corner = which_corner(pos);
		for(int x = 0; x < CHUNK_SIZE; x++)
			for(int y = 0; y < CHUNK_SIZE; y++)
			{
				if (corner == 1) // northeast
					if (x + y >= CHUNK_SIZE - 1)
						continue;
				if (corner == 2) // southwest
					if (x + y < CHUNK_SIZE)
						continue;

				Coord tile = Coord(x, y);
				Coord world = world_coord(pos, tile);
				bool f = (world != m);
				draw_hex(world, Piece(f ? chunk.get_at(tile) : -1));
			}

	}
#endif

#if DRAW_BG
	draw_empty_grid();
#endif

#if DRAW_CHUNK_BORDERS
	for (auto i : chunks)
	{
		Coord chunk = i.first;
		DrawRectangleLinesEx(chunk_border(chunk), 0.02, GetColor(0xffffffff));
	}
#endif

	if (chunks.empty())
	{
		draw_hex(Coord(0, 0), PIECE_NONE);
	}

#if DEBUG_TEXT
	rend.end_cam();
	DrawText(std::format("Chunks drawn: {}", count).c_str(), 5, 25, 20, GetColor(0xffffffff));
	rend.begin_cam();
#endif

}
