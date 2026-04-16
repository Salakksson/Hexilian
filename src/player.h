#ifndef PLAYER_H_
#define PLAYER_H_

#include "grid.h"
#include "renderer.h"

struct Player
{
	Coord move;
	bool move_ready;

	inline void step()
	{
		if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
		{
			Vector2 pos = rend.mouse_pos_world();
			move = vec_to_coord(pos);
			move_ready = true;
		}
	}

	inline Coord take_move()
	{
		move_ready = false;
		return move;
	}
};

#endif
