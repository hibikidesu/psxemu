#ifndef PSX_RENDERER
#define PSX_RENDERER

#include <stdint.h>
#include "microui.h"

#define VERTEX_BUFFER_LEN 64 * 1024

// Pos in VRAM
typedef struct {
	uint16_t x;
	uint16_t y;
} RendererPosition;
// Color
typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} RendererColor;

mu_Context *renderer_GetMUContext();
void renderer_DrawQuad(RendererPosition *positions, RendererColor *colors);
void renderer_DrawTriangle(RendererPosition *positions, RendererColor *colors);
RendererColor renderer_GetColorFromGP0(uint32_t value);
RendererPosition renderer_GetPositionFromGP0(uint32_t value);
void renderer_Update();
void renderer_Init();
void renderer_Destroy();

#endif