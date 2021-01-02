#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <SDL2/SDL_gpu.h>
#include "renderer.h"
#include "imagebuffer.h"
#include "../utils/logger.h"

static GPU_Target *g_Screen = NULL;
static GPU_Image *g_Image = NULL;
static GPU_Image *g_DrawingTexture = NULL;
static SDL_Surface *g_TempSurface = NULL;
static RendererPosition g_Clut;
static RendererPosition g_Page;
static RendererPosition g_TexCoords;
static uint8_t g_DrawTexture = 0;

void renderer_SetDrawTexture(uint8_t value) {
	g_DrawTexture = value;
}

void renderer_SetTexCoords(RendererPosition coords) {
	g_TexCoords = coords;
}

void renderer_SetPage(RendererPosition page) {
	g_Page = page;
}

void renderer_SetClut(RendererPosition clut) {
	g_Clut = clut;
}

uint16_t get_texel_4bit(IMAGEBUFFER *imageBuffer, uint16_t x, uint16_t y, RendererPosition clut, RendererPosition page) {
	uint16_t texel = imageBuffer->buffer[(page.x + x / 4) + (page.y + y) * 1024];
	uint32_t index = (texel >> (x % 4) * 4) % 0xF;
	return imageBuffer->buffer[(clut.x + index) + (clut.y) * 1024];
}

uint16_t get_texel_8bit(IMAGEBUFFER *imageBuffer, uint16_t x, uint16_t y, RendererPosition clut, RendererPosition page) {
	uint16_t texel = imageBuffer->buffer[(x / 2 + page.x) + (y + page.y) * 1024];
	uint32_t index = (texel >> (x % 2) * 8) % 0xFF;
	return imageBuffer->buffer[(clut.x + index) + (clut.y) * 1024];
}

uint16_t get_texel_16bit(IMAGEBUFFER *imageBuffer, uint16_t x, uint16_t y, RendererPosition clut, RendererPosition page) {
	return imageBuffer->buffer[(clut.x + page.x) + (clut.y + page.y) * 1024];
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

SDL_Surface *get_cropped_pixels_tex(IMAGEBUFFER *imageBuffer) {
	uint16_t x = g_Page.x * 64;
	uint16_t y = g_Page.y * 256;
	uint16_t w = 128;
	uint16_t h = 128;
	uint16_t buffer[w * h];
	int i, bufferPos;

	bufferPos = x + y * 1024;
	for (i = 0; i < (w * h); i++) {
		assert(i <= w * h);
		// Copy into local buffer from vram buffer
		buffer[i] = imageBuffer->buffer[bufferPos];
		bufferPos += 1;
		// if current index does a line
		if ((i % w) == 0) {
			// Seek to next line in vram buffer
			bufferPos = ((y + (i / w)) * 1024) + x;
		}
	}

	return SDL_CreateRGBSurfaceFrom((uint8_t*)buffer, w, h, 16, sizeof(uint16_t) * w, 0x1F, 0x3E0, 0x7c00, 0);
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
		GPU_Rect rect = GPU_MakeRect((float)g_Page.x * 64.f, (float)g_Page.x * 0.f, 128.f, 128.f);
		GPU_BlitScale(g_Image, &rect, g_Screen, (float)positions[0].x, (float)positions[0].y, (float)g_Page.x * 64.f / ((float)positions[1].x - (float)positions[0].x), 1.0f);
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
