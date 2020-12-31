#ifndef PSX_RENDERER
#define PSX_RENDERER

#include <stdint.h>

#define VERTEX_BUFFER_LEN 64 * 1024

// Pos in VRAM
struct RendererPosition {
	uint16_t x;
	uint16_t y;
};
// Color
struct RendererColor {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

void renderer_DrawTriangleShade(struct RendererPosition *positions, struct RendererColor *colors);
struct RendererColor renderer_GetColorFromGP0(uint32_t value);
struct RendererPosition renderer_GetPositionFromGP0(uint32_t value);
void renderer_Update();
void renderer_Init();
void renderer_Destroy();

#endif