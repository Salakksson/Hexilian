#ifndef ENGINE_H_
#define ENGINE_H_

#include "grid.h"
#include "player.h"

struct Move
{
	Coord c1;
	Coord c2;
	int eval;
};

struct Engine
{
	Move root_move;
	bool move_ready;

	Grid position;

	Piece p;

	static const int inf = 999999999;
	static const int max_depth = 2;

	int nodes;

	/* int[][] coord_history = new int[64, 64]; */
	/* move killer_moves[][] = new move[max_depth + 1, 2]; */

	Move search(int depth, int alpha, int beta);

	inline void think()
	{
		for(int i = 1; i <= max_depth; i++)
		{
			nodes = 0;
			TraceLog(LOG_WARNING, "depth %i:", i);
			root_move = search(i, -Engine::inf, Engine::inf);
			TraceLog(LOG_WARNING, "eval: %i", root_move.eval);
			TraceLog(LOG_WARNING, "nodes: %i", nodes);
		}
	}

	inline Move take_move()
	{
		move_ready = false;
		return root_move;
	}

	inline void update(Grid& new_position)
	{
		position = new_position.clone();
	}

	inline void update(Coord move)
	{
		position.play(move);
	}

	void record_killer_move(Move m, int depth);
	void record_history(Move m, int depth);
	int get_history(Move m);


};

int eval(const Grid& position);

#endif
