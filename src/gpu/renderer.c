#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "gl3w/gl3w.h"
#include <SDL2/SDL_gpu.h>
#include "renderer.h"
#include "../utils/logger.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>

static GPU_Target *g_Screen = NULL;
static ImGuiIO *imgui_io = NULL;

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

SDL_Window *getSDLWindow() {
	return SDL_GetWindowFromID(GPU_GetContextTarget()->context->windowID);
}

void renderer_Update() {
	// ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(getSDLWindow());
	GPU_Flip(g_Screen);
	GPU_Clear(g_Screen);
}

void renderer_Init() {
	// Init SDL_gpu
	g_Screen = GPU_Init(1024, 512, GPU_DEFAULT_INIT_FLAGS);
	if (g_Screen == NULL) {
		log_Error("Failed to init sdl-gpu: %s", SDL_GetError());
		exit(1);
	}
	
	if (gl3wInit()) {
		log_Error("Failed to init glew: %s");
		exit(1);
	}

	// Init IMGUI
	igCreateContext(NULL);
	imgui_io = igGetIO();

	// Get GPU Context and init imgui
	GPU_Context *gpu_ctx = GPU_GetContextTarget()->context;
	ImGui_ImplSDL2_InitForOpenGL(getSDLWindow(), gpu_ctx->context);
	ImGui_ImplOpenGL3_Init("#version 150");


	// Set style
	igStyleColorsDark(NULL);
}

void renderer_Destroy() {
	GPU_Quit();
	SDL_Quit();
}
