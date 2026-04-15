#ifndef RENDERER_H_
#define RENDERER_H_

#include <raylib.h>

struct Renderer
{
	float width;
	float height;

	Color color_bg = GetColor(0x1a1b26ff);

	Camera2D cam;
	Font font;

	bool cam_enabled;

	Rectangle world_viewport;

	// max and min zoom
	const float MAX_VIEW = 35.0f;
	const float MIN_VIEW = 2.0f;

	void reset();
	void update();
	void begin();
	void end();

	void begin_cam();
	void end_cam();

	void draw_text(int x, int y, const char* fmt, ...);

	Vector2 mouse_pos_world();

	Renderer(int x, int y);
	~Renderer();
};

extern Renderer rend;

#endif
