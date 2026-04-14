#ifndef ENGINE_H_
#define ENGINE_H_

#include "grid.h"
#include "player.h"

struct Engine
{
	Coord move;
	bool move_ready;

	inline void step()
	{

	}

	inline Coord take_move()
	{
		move_ready = false;
		return move;
	}
};

int eval(const Grid& position);

#endif
