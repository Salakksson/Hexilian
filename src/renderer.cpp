#include "renderer.h"

#include "config.h"

#include <math.h>
#include <stdio.h>
#include "vector2.h"

Renderer::Renderer(int x, int y)
{
	width = x;
	height = y;

	SetTraceLogLevel(LOG_WARNING);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	SetTargetFPS(60);

	InitWindow(width, height, "_");

	SetExitKey(KEY_NULL);

	font = LoadFontEx("font.ttf", 100, nullptr, 0);

	reset();
}

void Renderer::reset()
{
	cam.zoom = height / 10;
	cam.target = Vector2{0, 0};
	cam.offset = Vector2{width / 2.0f, height / 2.0f};
}

void Renderer::update()
{
	width = GetScreenWidth();
	height = GetScreenHeight();

	// handle scroll
	float wheel = GetMouseWheelMove();
	if (wheel != 0)
	{
		// TODO: nice zooming
		// Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), cam);

		float zoom = pow(1.2, GetMouseWheelMove());

		cam.zoom *= zoom;
	}

	// handle movement
	if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
		cam.target -= GetMouseDelta() / cam.zoom;

	cam.offset = Vector2{width / 2.0f, height / 2.0f};

	// recalculate world rect
	if (IsWindowResized() || true)
	{
		float world_width = width / cam.zoom;
		float world_height = height / cam.zoom;
		world_viewport = {
			.x = cam.target.x - world_width / 2,
			.y = cam.target.y - world_height / 2,
			.width = world_width,
			.height = world_height,
		};
	}

	// zoomed too far out
	if (cam.zoom < height / MAX_VIEW)
	{
		cam.zoom = height / MAX_VIEW;
	}
	// zoomed too far in
	if (cam.zoom > height / MIN_VIEW)
	{
		cam.zoom = height / MIN_VIEW;
	}
}

void Renderer::begin()
{
	this->update();

	BeginDrawing();

	ClearBackground(color_bg);
}

void Renderer::end()
{
	if (cam_enabled) EndMode2D();
	cam_enabled = false;

#if DRAW_FPS
	draw_text(5, 5, "FPS: %i", GetFPS());
#endif
	EndDrawing();
}

void Renderer::begin_cam()
{
	if (cam_enabled) return;

	BeginMode2D(this->cam);
	cam_enabled = true;
}

void Renderer::end_cam()
{
	if (!cam_enabled) return;

	EndMode2D();
	cam_enabled = false;
}

Vector2 Renderer::world_to_screen(Vector2 vec)
{
	return GetWorldToScreen2D(vec, cam);
}

Vector2 Renderer::screen_to_world(Vector2 vec)
{
	return GetScreenToWorld2D(vec, cam);
}

Vector2 Renderer::mouse_pos_world()
{
	Vector2 pos = GetMousePosition();
	return GetScreenToWorld2D(pos, cam);
}

void Renderer::draw_text(int x, int y, const char* fmt, ...)
{
	bool cam = cam_enabled;
	end_cam();

	va_list args;
	va_list copy;
	va_start(args, fmt);
	va_copy(copy, args);

	size_t size = vsnprintf(0, 0, fmt, args) + 1;
	char* text = (char*)alloca(size);
	vsnprintf(text, size, fmt, copy);

	Vector2 position = {(float)x, (float)y};
	DrawTextEx(font, text, position, 30, 0, GetColor(0xc0caf5ff));
	va_end(args);
	va_end(copy);

	if (cam) begin_cam();
}

Renderer::~Renderer()
{
	CloseWindow();
}
