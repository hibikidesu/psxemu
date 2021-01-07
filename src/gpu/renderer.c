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
static SDL_Surface *g_TempDrawSurface = NULL;
static SDL_Surface *g_TempSurface = NULL;
static RendererPosition g_TexCoords;
static RendererPosition g_PaletteCords;
static RendererPosition g_PageCoords;
static uint8_t g_DrawTexture = 0;
static int g_TexDepth = 0;

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

void renderer_SetPageCoords(RendererPosition coords) {
	g_PageCoords = coords;
}

void renderer_SetTextureDepth(int depth) {
	switch (depth) {
		case 0:
			g_TexDepth = 4;
			break;
		case 1:
			g_TexDepth = 8;
			break;
		case 2:
			g_TexDepth = 16;
			break;
		default:
			break;
	}
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

void load_draw_img(IMAGEBUFFER *imageBuffer) {
	uint16_t x_o = g_PageCoords.x * 64;
	uint16_t y_o = g_PageCoords.y * 256;
	uint16_t w = g_TexDepth * 8;
	uint16_t h = g_TexDepth * 8;
	uint16_t buffer[w * h];
	int i = 0;
	int x, y;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			assert(i <= w * h);
			buffer[i] = imageBuffer_Read(imageBuffer, x + x_o, y + y_o);
			i += 1;
		}
	}

	g_TempDrawSurface = SDL_CreateRGBSurfaceFrom((uint8_t*)buffer, w, h, 16, sizeof(uint16_t) * w, 0x1F, 0x3E0, 0x7c00, 0);
	g_DrawingTexture = GPU_CopyImageFromSurface(g_TempDrawSurface);
	SDL_FreeSurface(g_TempDrawSurface);
}

void renderer_LoadImage(IMAGEBUFFER *imageBuffer) {
	// Free if already exists
	if (g_Image != NULL) {
		GPU_FreeImage(g_Image);
	}
	// Update VRAM
	renderer_GenerateSurface(imageBuffer);
	g_Image = GPU_CopyImageFromSurface(g_TempSurface);
}

void renderer_DrawQuad(IMAGEBUFFER *imageBuffer, RendererPosition *positions, RendererColor *colors) {
	uint32_t i;
	uint32_t n = 0;
	static const float st_v1[3][2] = {
		{0, 0},
		{1, 0},
		{0, 1}
	};
	static const float st_v2[3][2] = {
		{1, 0},
		{0, 1},
		{1, 1}
	};

	if (g_DrawTexture) {
		float values[6 * (2 + 4 + 2)];
		for (i = 0; i < 3; i++) {
			// XY
			values[n++] = (float)positions[i].x;
			values[n++] = (float)positions[i].y;
			// ST
			values[n++] = st_v1[i][0];
			values[n++] = st_v1[i][1];
			// RGBA
			values[n++] = (float)colors[i].r / 255;
			values[n++] = (float)colors[i].g / 255;
			values[n++] = (float)colors[i].b / 255;
			values[n++] = 1.0;
		}
		for (i = 1; i < 4; i++) {
			// XY
			values[n++] = (float)positions[i].x;
			values[n++] = (float)positions[i].y;
			// ST
			values[n++] = st_v2[i - 1][0];
			values[n++] = st_v2[i - 1][1];
			// RGBA
			values[n++] = (float)colors[i].r / 255;
			values[n++] = (float)colors[i].g / 255;
			values[n++] = (float)colors[i].b / 255;
			values[n++] = 1.0;
		}
		load_draw_img(imageBuffer);
		GPU_TriangleBatch(g_DrawingTexture, g_Screen, 6, values, 0, NULL, GPU_BATCH_XY_ST_RGBA);
		g_DrawTexture = 0;
	} else {
		float values[6 * (2 + 4)];
		for (i = 0; i < 3; i++) {
			// XY
			values[n++] = (float)positions[i].x;
			values[n++] = (float)positions[i].y;
			// RGBA
			values[n++] = (float)colors[i].r / 255;
			values[n++] = (float)colors[i].g / 255;
			values[n++] = (float)colors[i].b / 255;
			values[n++] = 1.0;
		}
		for (i = 1; i < 4; i++) {
			// XY
			values[n++] = (float)positions[i].x;
			values[n++] = (float)positions[i].y;
			// RGBA
			values[n++] = (float)colors[i].r / 255;
			values[n++] = (float)colors[i].g / 255;
			values[n++] = (float)colors[i].b / 255;
			values[n++] = 1.0;
		}
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
	SDL_FreeSurface(g_TempDrawSurface);
	GPU_FreeImage(g_Image);
	GPU_Quit();
	SDL_Quit();
}
