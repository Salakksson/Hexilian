#include "engine.h"

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

#include <unordered_set>

int eval_sign(Piece p)
{
	switch (p)
	{
	case Piece::none: return 0;
	case Piece::cross: return 1;
	case Piece::circle: return -1;
	default: return 0;
	}
}

int eval(Line l, Piece owner, int moveno)
{
	int basic = 0;

	switch (l.length)
	{
	case 2: basic = 100; break;
	case 3: basic = 800; break;
	case 4: basic = 4600; break;
	case 5: basic = 6800; break;
	case 6: basic = Engine::inf; break;
	default: return 0;
	}

	basic *= eval_sign(owner);

	int age = moveno - l.moveno_added;

	return basic - age;
}

int eval(Grid& position, Piece perspective)
{
	int sum = 0;
	for (Line l : position.lines)
	{
		sum += eval(l, position.get_at(l.start), position.moveno);
	}

	return sum * eval_sign(perspective);
}

void add_border(
	Grid& position,
	std::unordered_set<Coord>& coords,
	Coord center,
	int radius
) {
	for (int x = -radius; x <= radius; x++)
	{
		for (int y = -radius; y <= radius; y++)
		{
			Coord c = center + Coord(x, y);
			if (position.is_empty(c))
			coords.insert(c);
		}
	}

}

std::unordered_set<Coord> reasonable_coords(Grid& position)
{
	std::unordered_set<Coord> coords;

	for (Coord c : position.moves)
	{
		add_border(position, coords, c, 1);
	}

	return coords;
}

void update_reasonable_coords(std::unordered_set<Coord>& coords, Grid& position)
{
	add_border(position, coords, position.moves.back(), 1);
}

Move Engine::search(int depth, int alpha, int beta)
{
	nodes++;
	if (depth <= 0 || position.is_won)
		return {Coord(), Coord(), eval(position, p)};

	Move best = {Coord(), Coord(), -Engine::inf};

	std::unordered_set<Coord> coords = reasonable_coords(position);
	add_border(position, coords, Coord(0, 0), 1);
	if (coords.size() < 2)
	{
		TraceLog(LOG_ERROR, "NO MOVES FOUND");
	}

	for (Coord c1 : coords)
	{
		if (!position.is_empty(c1)) continue;
		for (Coord c2 : coords)
		{
			if (!position.is_empty(c2)) continue;
			if (c2 == c1) continue;

			position.play(c1);
			position.play(c2);
			int eval = -search(depth - 1, -beta, -alpha).eval;
			position.unplay();
			position.unplay();
			if (eval > best.eval)
				best = {c1, c2, eval};
			if (eval > alpha)
			{
				alpha = eval;
				best = {c1, c2, eval};
			}
			if (alpha >= beta)
			{
				break;
			}
		}
	}

	return best;
}


// void Engine::record_killer_move(move m, int depth)
// {
//	if (position.get_piece(m.square_to) != piece.none) return;
//	if (killer_moves[depth, 0] == m) return;
//	killer_moves[depth, 1] = killer_moves[depth, 0];
//	killer_moves[depth, 0] = m;
// }

// void Engine::record_history(move m, int depth)
// {
//	move_history[m.square_from, m.square_to] += depth * depth;
// }

// int Engine::get_history(move m)
// {
//	return move_history[m.square_from, m.square_to];
// }

