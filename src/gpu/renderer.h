#ifndef PSX_RENDERER
#define PSX_RENDERER

#include <stdint.h>
#include "imagebuffer.h"

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

void renderer_SetTextureDepth(int depth);
void renderer_SetPageCoords(RendererPosition coords);
void renderer_SetDrawTexture(uint8_t value);
void renderer_SetTexCoords(RendererPosition coords);
void renderer_SetPaletteCoords(RendererPosition coords);
uint16_t get_texel_4bit(IMAGEBUFFER *imageBuffer, uint16_t x, uint16_t y, RendererPosition clut, RendererPosition page);
uint16_t get_texel_8bit(IMAGEBUFFER *imageBuffer, uint16_t x, uint16_t y, RendererPosition clut, RendererPosition page);
uint16_t get_texel_16bit(IMAGEBUFFER *imageBuffer, uint16_t x, uint16_t y, RendererPosition clut, RendererPosition page);
void renderer_GenerateSurface(IMAGEBUFFER *imageBuffer);
void renderer_LoadImage(IMAGEBUFFER *imageBuffer);
void renderer_DrawRect(RendererPosition *position, RendererColor color, uint8_t alpha);
void renderer_DrawQuad(IMAGEBUFFER *imageBuffer, RendererPosition *positions, RendererColor *colors);
void renderer_DrawTriangle(RendererPosition *positions, RendererColor *colors);
RendererColor renderer_GetColorFromGP0(uint32_t value);
RendererPosition renderer_GetPositionFromGP0(uint32_t value);
void renderer_Update();
void renderer_Init();
void renderer_Destroy();

#endif