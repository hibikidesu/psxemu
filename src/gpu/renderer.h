#ifndef PSX_RENDERER
#define PSX_RENDERER

#include <stdint.h>

// Pos in VRAM
typedef struct {
	uint16_t x;
	uint16_t y;
} RendererPositon;
// Color
typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} RendererColor;

void renderer_DrawTriangle(RendererPositon **positions, RendererColor **colors, uint32_t size);
RendererColor *renderer_GetColorFromGP0(uint32_t value);
RendererPositon *renderer_GetPositionFromGP0(uint32_t value);
void renderer_Update();
void renderer_Init();
void renderer_Destroy();

#endif