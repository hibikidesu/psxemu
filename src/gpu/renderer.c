#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_gpu.h>
#include "renderer.h"
#include "../utils/logger.h"

static SDL_Window *g_Window = NULL;

// void interpolate(RendererColor *color1, RendererColor *color2, float fraction) {
// 	uint8_t r = (uint8_t)((color2->r - color1->r) * fraction + color1->r);
// 	uint8_t g = (uint8_t)((color2->g - color1->g) * fraction + color1->g);
// 	uint8_t b = (uint8_t)((color2->b - color1->b) * fraction + color1->b);
// 	SDL_SetRenderDrawColor(g_Renderer, r, g, b, 255);
// }

// void algoDDA(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2) {
// 	float dx = (x2 - x1);
// 	float dy = (y2 - y1);
// 	float step;
// 	float x, y;
// 	int i;

// 	if (abs(dx) >= abs(dy)) {
// 		step = abs(dx);
// 	} else {
// 		step = abs(dy);
// 	}

// 	dx = (dx / step);
// 	dy = (dy / step);
// 	x = x1;
// 	y = y1;
// 	for (i = 0; i < step; i++) {
// 		SDL_RenderDrawPoint(g_Renderer, (int)x, (int)y);
// 		x += dx;
// 		y += dy;
// 	}
// }

void renderer_DrawTriangleShade(struct RendererPosition *positions, struct RendererColor *colors) {

}

struct RendererColor renderer_GetColorFromGP0(uint32_t value) {
	struct RendererColor color;
	color.r = (uint8_t)value;
	color.g = (uint8_t)(value >> 8);
	color.b = (uint8_t)(value >> 16);
	return color;
}

struct RendererPosition renderer_GetPositionFromGP0(uint32_t value) {
	struct RendererPosition pos;
	pos.x = (int16_t)value;
	pos.y = (int16_t)(value >> 16);
	return pos;
}

void renderer_Update() {
	
}

void renderer_Init() {
	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		log_Error("Failed to init SDL2: %s", SDL_GetError());
		exit(1);
	}

	g_Window = SDL_CreateWindow("uwu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 512, SDL_WINDOW_OPENGL);
	if (g_Window == NULL) {
		log_Error("Failed to create window: %s", SDL_GetError());
		exit(1);
	}

	// Wait for it to be ready
	SDL_Delay(80);

	renderer_Update();
}

void renderer_Destroy() {
	SDL_DestroyWindow(g_Window);
	SDL_Quit();
}
