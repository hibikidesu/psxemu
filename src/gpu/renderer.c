#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL_gpu.h>
#include <SDL2/SDL_ttf.h>
#include "renderer.h"
#include "../utils/logger.h"
#include "microui.h"

static GPU_Target *g_Screen = NULL;
static mu_Context *mu_ctx = NULL;
static TTF_Font *g_Font = NULL;

void renderer_DrawQuad(RendererPosition *positions, RendererColor *colors) {
	// vertices * (x, y + r, g, b, a)
	float values[6 * (2 + 4)] = {
		// 1st Position
		// X, Y
		(float)positions[0].x,
		(float)positions[0].y,
		// RGBA
		(float)colors[0].r / 255,
		(float)colors[0].g / 255,
		(float)colors[0].b / 255,
		1.0,
		// 2nd Position
		// X, Y
		(float)positions[1].x,
		(float)positions[1].y,
		// RGBA
		(float)colors[1].r / 255,
		(float)colors[1].g / 255,
		(float)colors[1].b / 255,
		1.0,
		// 3rd Position
		// X, Y
		(float)positions[2].x,
		(float)positions[2].y,
		// RGBA
		(float)colors[2].r / 255,
		(float)colors[2].g / 255,
		(float)colors[2].b / 255,
		1.0,
		// 1st Position, T2
		// X, Y
		(float)positions[1].x,
		(float)positions[1].y,
		// RGBA
		(float)colors[1].r / 255,
		(float)colors[1].g / 255,
		(float)colors[1].b / 255,
		1.0,
		// 2nd Position, T2
		// X, Y
		(float)positions[2].x,
		(float)positions[2].y,
		// RGBA
		(float)colors[2].r / 255,
		(float)colors[2].g / 255,
		(float)colors[2].b / 255,
		1.0,
		// 3rd Position, T2
		// X, Y
		(float)positions[3].x,
		(float)positions[3].y,
		// RGBA
		(float)colors[3].r / 255,
		(float)colors[3].g / 255,
		(float)colors[3].b / 255,
		1.0,
	};
	GPU_TriangleBatch(NULL, g_Screen, 6, values, 0, NULL, GPU_BATCH_XY_RGBA);
}

void renderer_DrawTriangle(RendererPosition *positions, RendererColor *colors) {
	// vertices * (x, y + r, g, b, a)
	float values[3 * (2 + 4)] = {
		// 1st Position
		// X, Y
		(float)positions[0].x,
		(float)positions[0].y,
		// RGBA
		(float)colors[0].r / 255,
		(float)colors[0].g / 255,
		(float)colors[0].b / 255,
		1.0,
		// 2nd Position
		// X, Y
		(float)positions[1].x,
		(float)positions[1].y,
		// RGBA
		(float)colors[1].r / 255,
		(float)colors[1].g / 255,
		(float)colors[1].b / 255,
		1.0,
		// 3rd Position
		// X, Y
		(float)positions[2].x,
		(float)positions[2].y,
		// RGBA
		(float)colors[2].r / 255,
		(float)colors[2].g / 255,
		(float)colors[2].b / 255,
		1.0,
	};
	GPU_TriangleBatch(NULL, g_Screen, 3, values, 0, NULL, GPU_BATCH_XY_RGBA);
}

RendererColor renderer_GetColorFromGP0(uint32_t value) {
	RendererColor color;
	color.r = (uint8_t)value;
	color.g = (uint8_t)(value >> 8);
	color.b = (uint8_t)(value >> 16);
	return color;
}

RendererPosition renderer_GetPositionFromGP0(uint32_t value) {
	RendererPosition pos;
	pos.x = (int16_t)value;
	pos.y = (int16_t)(value >> 16);
	return pos;
}

void process_frame() {
	mu_begin(mu_ctx);
	// style_window(mu_ctx);
	// log_window(mu_ctx);
	// test_window(mu_ctx);
	mu_end(mu_ctx);
}

void draw_text(mu_Font font, const char *text, mu_Vec2 pos, mu_Color color) {
	
}

void handle_commands() {
	mu_Command *cmd = NULL;
	while (mu_next_command(mu_ctx, &cmd)) {
		switch (cmd->type) {
			case MU_COMMAND_TEXT:
				draw_text(cmd->text.font, cmd->text.str, cmd->text.pos, cmd->text.color);
				break;
			default:
				break;
		}
	}
}

void renderer_Update() {
	process_frame();
	handle_commands();
	GPU_Flip(g_Screen);
	GPU_Clear(g_Screen);
}

static int text_width(mu_Font font, const char *text, int len) {
	
	return 1;
}

static int text_height(mu_Font font) {
	return 18;
}

mu_Context *renderer_GetMUContext() {
	return mu_ctx;
}

void renderer_Init() {
	// Init SDL_gpu
	g_Screen = GPU_Init(1024, 512, GPU_DEFAULT_INIT_FLAGS);
	if (g_Screen == NULL) {
		log_Error("Failed to init sdl-gpu: %s", SDL_GetError());
		exit(1);
	}

	// Init TTF
	if (TTF_Init() == -1) {
		log_Error("Failed to init SDL2_ttf: %s", SDL_GetError());
		exit(1);
	}

	// Load font
	g_Font = TTF_OpenFont("fonts/Roboto/Roboto-Regular.ttf", 12);
	if (g_Font == NULL) {
		log_Error("Failed to load font: %s", SDL_GetError());
		exit(1);
	}

	// Init microui
	mu_ctx = malloc(sizeof(mu_Context));
	mu_init(mu_ctx);
	mu_ctx->text_width = text_width;
	mu_ctx->text_height = text_height;
}

void renderer_Destroy() {
	TTF_Quit();
	GPU_Quit();
	SDL_Quit();
}
