#include "grid.h"

#include "renderer.h"
#include "engine.h"

#include <stack>

Renderer rend(1280, 720);

struct Game
{
	Grid grid;
	Engine engine;
	Human human;

	std::stack<Coord> past_moves;
	void tick()
	{
		if (IsKeyPressed(KEY_U) && !past_moves.empty())
		{
			grid.unplay(past_moves.top());
			past_moves.pop();
		}
		human.step();
		if (human.move_ready)
		{
			Coord move = human.take_move();

			grid.play(move);
			past_moves.push(move);
		}
	}
};

int main()
{
	Game game;

	while (!WindowShouldClose())
	{
		rend.begin();
		rend.begin_cam();

#if SMALL_VIEWPORT
		Rectangle rec = rend.world_viewport;
		rec.x += 1;
		rec.y += 1;
		rec.width -= 2;
		rec.height -= 2;
		DrawRectangleRec(rec, GetColor(0x002020ff));
		DrawRectangleLinesEx(rec, 0.05, GetColor(0xffffffff));
		rend.world_viewport = rec;
#endif

		game.tick();
		game.grid.draw();

		rend.end_cam();


		rend.end();
	}
}
