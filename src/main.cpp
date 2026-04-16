#include "grid.h"

#include "renderer.h"
#include "engine.h"

#include <stack>

Renderer rend(1280, 720);

struct Game
{
	Grid grid;
	Engine engine;
	Player player;

	void tick()
	{
		if (IsKeyPressed(KEY_U))
		{
			TraceLog(LOG_WARNING, "undoing");
			grid.unplay();
		}
		if (grid.turn == Piece::circle)
		{
			engine.update(grid);
			engine.think();
			Move m = engine.take_move();
			if (m.c1 == Coord() || m.c2 == Coord())
			{
				TraceLog(LOG_WARNING, "no move made");
			}
			grid.play(m.c1);
			grid.play(m.c2);
		}
		else
		{
			player.step();
			if (player.move_ready)
			{
				Coord move = player.take_move();

				grid.play(move);
			}
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
