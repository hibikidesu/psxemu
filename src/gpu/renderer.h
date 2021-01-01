#ifndef PSX_RENDERER
#define PSX_RENDERER

#include <stdint.h>

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

void renderer_DrawRect(RendererPosition *position, RendererColor color, uint8_t alpha);
void renderer_DrawQuad(RendererPosition *positions, RendererColor *colors);
void renderer_DrawTriangle(RendererPosition *positions, RendererColor *colors);
RendererColor renderer_GetColorFromGP0(uint32_t value);
RendererPosition renderer_GetPositionFromGP0(uint32_t value);
void renderer_Update();
void renderer_Init();
void renderer_Destroy();

#endif