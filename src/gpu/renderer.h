#ifndef PSX_RENDERER
#define PSX_RENDERER

#include <stdint.h>
#include <GL/gl.h>

#define VERTEX_BUFFER_LEN 64 * 1024

// Pos in VRAM
typedef struct {
	GLshort x;
	GLshort y;
} RendererPosition;
// Color
typedef struct {
	GLubyte r;
	GLubyte g;
	GLubyte b;
} RendererColor;

void renderer_DrawTriangleShade(RendererPosition *positions, RendererColor *colors);
RendererColor renderer_GetColorFromGP0(uint32_t value);
RendererPosition renderer_GetPositionFromGP0(uint32_t value);
void renderer_Update();
void renderer_Init();
void renderer_Destroy();

#endif