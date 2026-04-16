#include "grid.h"

#include <raylib.h>

#include "renderer.h"
#include "vector2.h"

constexpr float SQRT3 = std::sqrt(3);

constexpr Coord hex_directions[6] = {
	// Where z = x - y:
	{1, 0},  // +x
	{0, 1},  // +y
	{1, -1}, // +z
	{-1, 0}, // -x
	{0, -1}, // -y
	{-1, 1}, // -z
};

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
	case Piece::none: return GetColor(0x1a1b26ff);
	case Piece::cross: return GetColor(0xf7768eff);
	case Piece::circle: return GetColor(0x0db9d7ff);
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
				GetColor(0xc0caf53f)
			);
			// Vector2 text_pos = rend.world_to_screen(pos);
			// rend.draw_text(
			//	text_pos.x - 10,
			//	text_pos.y - 10,
			//	"%i",
			//	coord.dist()
			// );
		}
	}
#if DEBUG_TEXT
	rend.draw_text(5, 45, "Hexagons drawn: %i", count);
#endif
}


void Grid::draw_hex(Coord coord, Piece p)
{
	coord -= offset;

	if (p == Piece::none) return;
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

void Grid::add_lines(Coord center, Piece added)
{
	// remove any lines we are connecting with
	std::erase_if(lines, [this, center, added](Line& l) {

		if (!is_equal(l.start, added)) return false;

		if (l.start == center + l.dir)
		{
			l.moveno_removed = this->moveno;
			this->old_lines.push_back(l);
			return true;
		}


		if (l.end == center - l.dir)
		{
			l.moveno_removed = this->moveno;
			this->old_lines.push_back(l);
			return true;
		}

		return false;
	});

	// only iterate over "positive" axis
	// so that Line.dir is always "positive"
	for (int i = 0; i < 3; i++)
	{
		Coord dir = hex_directions[i];

		int len = 1;
		Coord start = center;
		Coord end = center;

		// extend in positive direction
		for (int j = 1; j < 6; j++)
		{
			Coord coord = center + (dir * j);
			if (!is_equal(coord, added)) break;

			// extend
			end = coord;
			len++;
		}

		// negative
		for (int j = 1; j < 6; j++)
		{
			Coord coord = center - (dir * j);
			if (!is_equal(coord, added)) break;

			// extend
			start = coord;
			len++;
		}

		if (len > 1) {
			Line l = {
				.moveno_added = moveno,
				.length = len,
				.start = start,
				.end = end,
				.dir = dir,
			};
			lines.push_back(l);

			if (len >= 6) is_won = true;
		}
	}
}

void Grid::remove_lines(Coord center, Piece removed)
{
	// lines that are being removed
	std::erase_if(lines, [this](Line l) {
		return l.moveno_added == this->moveno;
	});

	// lines that are being re-added
	std::erase_if(old_lines, [this](Line l) {
		if (l.moveno_removed == this->moveno)
		{
			this->lines.push_back(l);
			return true;
		}
		return false;
	});
}

void Grid::update_lines(Coord center, Piece owner)
{
	if (get_at(center) == Piece::none)
		remove_lines(center, owner);
	else
		add_lines(center, owner);
}

bool Grid::is_empty(Coord coord)
{
	if (!chunks.contains(coord.chunk())) return true;
	Piece p = get_at(coord);
	return (p == Piece::none);
}

bool Grid::is_equal(Coord coord, Piece piece)
{
	if (!chunks.contains(coord.chunk())) return false;
	Piece p = get_at(coord);
	return (p == piece);
}

// void Grid::place(Coord coord, Piece p)
// {
//	TraceLog(LOG_WARNING, "Should not use Grid::place");
//	coord += offset;
// #if TOUCH_ON_PLACE
//	touch_chunk(coord.chunk());
// #endif
//	chunks[coord.chunk()].set_at(coord.tile(), p);

//	update_lines(coord, p);
// }

void Grid::play(Coord coord)
{
	coord += offset;
	// tile already taken
	if (!is_empty(coord))
		return;

#if TOUCH_ON_PLACE
	touch_chunk(coord.chunk());
#endif
	chunks[coord.chunk()].set_at(coord.tile(), turn);

	update_lines(coord, turn);

	n_turns--;
	if (n_turns == 0)
	{
		n_turns = 2;
		switch (turn)
		{
		case Piece::cross: turn = Piece::circle; break;
		case Piece::circle: turn = Piece::cross; break;
		default: turn = Piece::none; break;
		}
	}

	moves.push_back(coord);
	moveno++;
}

void Grid::unplay(Coord coord)
{
	coord += offset;

	if (is_empty(coord))
		return;

	chunks[coord.chunk()].set_at(coord.tile(), Piece::none);

	if (n_turns == 2)
	{
		n_turns = 0;
		switch (turn)
		{
		case Piece::cross: turn = Piece::circle; break;
		case Piece::circle: turn = Piece::cross; break;
		default: turn = Piece::none; break;
		}
	}

	n_turns++;

	moveno--;
	update_lines(coord, turn);

	is_won = false;
	moves.pop_back();
}

void Grid::unplay()
{
	if (moves.empty()) return;
	Coord move = moves.back();
	unplay(move);
}

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

inline Rectangle chunk_box(Coord chunk)
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
		chunk_box(chunk),
		rend.world_viewport
	);
#else
	return true;
#endif
}

void Grid::draw()
{
	// Vector2 mouse = rend.mouse_pos_world();
	// Coord m = vec_to_coord(mouse);

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

				draw_hex(world, chunk.get_at(tile));
			}

	}
#endif

#if DRAW_BG
	draw_empty_grid();
#endif

#if DRAW_CHUNK_BOXES
	for (auto i : chunks)
	{
		Coord chunk = i.first;
		DrawRectangleLinesEx(chunk_box(chunk), 0.02, WHITE);
	}
#endif

#if DRAW_CHUNK_BORDERS
	for (auto i : chunks)
	{
		Coord p1 = i.first * CHUNK_SIZE - offset;
		Coord p2 = Coord(p1.x + CHUNK_SIZE - 1, p1.y);
		Coord p3 = Coord(p1.x + CHUNK_SIZE - 1, p1.y + CHUNK_SIZE - 1);
		Coord p4 = Coord(p1.x, p1.y + CHUNK_SIZE - 1);
		DrawLineEx(coord_center(p1), coord_center(p2), 0.02, WHITE);
		DrawLineEx(coord_center(p2), coord_center(p3), 0.02, WHITE);
		DrawLineEx(coord_center(p3), coord_center(p4), 0.02, WHITE);
		DrawLineEx(coord_center(p4), coord_center(p1), 0.02, WHITE);
	}
#endif

#if DRAW_LINES
	for (Line l : lines)
	{
		DrawLineEx(
			coord_center(l.start - offset),
			coord_center(l.end - offset),
			0.1,
			RED
		);
	}
#endif
	if (chunks.empty())
	{
		draw_hex(Coord(0, 0), Piece::none);
	}

#if DEBUG_TEXT
	rend.draw_text(5, 25, "Chunks drawn: %i", count);
	rend.draw_text(5, 65, "Lines: %i", lines.size());
#endif

}
