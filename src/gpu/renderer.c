#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <SDL2/SDL_gpu.h>
#include "renderer.h"
#include "imagebuffer.h"
#include "debugger.h"
#include "../utils/logger.h"

static GPU_Target *g_Screen = NULL;
static GPU_Image *g_Image = NULL;
static GPU_Image *g_DrawingTexture = NULL;
static SDL_Surface *g_TempSurface = NULL;
static RendererPosition g_TexCoords;
static RendererPosition g_PaletteCords;
static uint8_t g_DrawTexture = 0;

RendererColor uint16_to_color(uint16_t value) {
	RendererColor color;
	color.r = (value << 3) & 0xf8;
	color.g = (value >> 2) & 0xf8;
	color.b = (value >> 7) & 0xf8;
	return color;
}

RendererColor get_color_16(IMAGEBUFFER *imageBuffer, uint32_t x, uint32_t y) {
	return uint16_to_color(imageBuffer_Read(imageBuffer, g_TexCoords.x + x, g_TexCoords.y + y));
}

RendererColor get_color_8(IMAGEBUFFER *imageBuffer, uint32_t x, uint32_t y) {
	uint16_t texel = imageBuffer_Read(imageBuffer, g_TexCoords.x + x / 2, g_TexCoords.y + y);
	int index = (texel >> (x & 1) * 8) & 255;
	uint16_t pixel = imageBuffer_Read(imageBuffer, g_TexCoords.x + index, g_TexCoords.y);
	return uint16_to_color(pixel);
}

RendererColor get_color_4(IMAGEBUFFER *imageBuffer, uint32_t x, uint32_t y) {
	uint16_t texel = imageBuffer_Read(imageBuffer, g_TexCoords.x + x / 4, g_TexCoords.y + y);
	int index = (texel >> (x & 3) * 4) & 15;
	uint16_t pixel = imageBuffer_Read(imageBuffer, g_TexCoords.x + index, g_TexCoords.y);
	return uint16_to_color(pixel);
}

void renderer_SetDrawTexture(uint8_t value) {
	g_DrawTexture = value;
}

void renderer_SetTexCoords(RendererPosition coords) {
	g_TexCoords = coords;
}

void renderer_SetPaletteCoords(RendererPosition coords) {
	g_PaletteCords = coords;
}

void renderer_GenerateSurface(IMAGEBUFFER *imageBuffer) {
	if (g_TempSurface != NULL) {
		SDL_FreeSurface(g_TempSurface);
	}
	g_TempSurface = SDL_CreateRGBSurfaceFrom((uint8_t*)imageBuffer->buffer, 
											 1024,
											 512,
											 16,
											 sizeof(uint16_t) * 1024,
											 0x1F, 0x3E0, 0x7c00, 0);
}

void renderer_LoadImage(IMAGEBUFFER *imageBuffer) {
	// Free if already exists
	if (g_Image != NULL) {
		GPU_FreeImage(g_Image);
	}
	// Update VRAM
	renderer_GenerateSurface(imageBuffer);
	g_Image = GPU_CopyImageFromSurface(g_TempSurface);

	SDL_Surface *sur = get_cropped_pixels_tex(imageBuffer);
	g_DrawingTexture = GPU_CopyImageFromSurface(sur);
	SDL_FreeSurface(sur);
}

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

	if (g_DrawTexture) {
		GPU_TriangleBatch(g_DrawingTexture, g_Screen, 6, values, 0, NULL, GPU_BATCH_XY_RGBA);
		g_DrawTexture = 0;
	} else {
		GPU_TriangleBatch(NULL, g_Screen, 6, values, 0, NULL, GPU_BATCH_XY_RGBA);
	}
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

void renderer_DrawRect(RendererPosition *position, RendererColor color, uint8_t alpha) {
	GPU_Rectangle(g_Screen, position[0].x, position[0].y, position[1].x, position[1].y, (SDL_Color){color.r, color.g, color.b, alpha});
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

void renderer_Update() {
	int i;
	// GPU_Blit(g_DrawingTexture, NULL, g_Screen, 512, 256);
	gpudebugger_RenderFrame(g_Screen);
	GPU_Flip(g_Screen);
	GPU_Clear(g_Screen);
	GPU_Blit(g_Image, NULL, g_Screen, 512, 256);
	for (i = 0; i < 3; i++) {
		GPU_Rectangle(g_Screen, 640 + (i * 128), 0, 640 + ((i + 1) * 128), 128, (SDL_Color){255, 0, 255, 255});
	}
}

void renderer_Init() {
	// Init SDL_gpu
	g_Screen = GPU_Init(1024, 512, GPU_DEFAULT_INIT_FLAGS);
	if (g_Screen == NULL) {
		log_Error("Failed to init sdl-gpu: %s", SDL_GetError());
		exit(1);
	}

	GPU_SetShapeBlending(1);
}

void renderer_Destroy() {
	GPU_FreeImage(g_DrawingTexture);
	SDL_FreeSurface(g_TempSurface);
	GPU_FreeImage(g_Image);
	GPU_Quit();
	SDL_Quit();
}
