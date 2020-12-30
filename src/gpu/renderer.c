#include <stdlib.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "renderer.h"
#include "../utils/logger.h"

static SDL_Window *g_Window = NULL;
static SDL_Renderer *g_Renderer = NULL;

void renderer_DrawTriangle(RendererPositon **positions, RendererColor **colors, uint32_t size) {

}

RendererColor *renderer_GetColorFromGP0(uint32_t value) {
	RendererColor *color = malloc(sizeof(RendererColor));
	color->r = (uint8_t)value;
	color->g = (uint8_t)(value >> 8);
	color->b = (uint8_t)(value >> 16);
	return color;
}

RendererPositon *renderer_GetPositionFromGP0(uint32_t value) {
	RendererPositon *pos = malloc(sizeof(RendererPositon));
	pos->x = (int16_t)value;
	pos->y = (int16_t)(value >> 16);
	return pos;
}

void renderer_Update() {
	SDL_RenderPresent(g_Renderer);
}

void renderer_Init() {
	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		log_Error("Failed to init SDL2: %s", SDL_GetError());
		exit(1);
	}

	g_Window = SDL_CreateWindow("zxc", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 512, SDL_WINDOW_SHOWN);
	if (g_Window == NULL) {
		log_Error("Failed to create window: %s", SDL_GetError());
		exit(1);
	}

	g_Renderer = SDL_CreateRenderer(g_Window, -1, SDL_RENDERER_ACCELERATED);
	if (g_Renderer == NULL) {
		log_Error("Failed to create renderer: %s", SDL_GetError());
		exit(1);
	}

	// Wait for it to be ready
	SDL_Delay(80);

	SDL_SetRenderDrawColor(g_Renderer, 0, 0, 0, 255);
	SDL_RenderClear(g_Renderer);
	renderer_Update();
}

void renderer_Destroy() {
	SDL_DestroyRenderer(g_Renderer);
	SDL_DestroyWindow(g_Window);
	SDL_Quit();
}
