#ifndef PSX_IMAGEBUFFER
#define PSX_IMAGEBUFFER

#include <stdint.h>

// 32 texture pages which can store 64x256 images in vram
#define MAX_IMAGE_SIZE 64 *  256
#define IMAGEBUFFER_MAX 32 * MAX_IMAGE_SIZE

typedef struct {
	uint16_t x, y;
	uint16_t w, h;
	uint32_t index;
	// Buffer used for temp storing of single images
	uint16_t buffer[IMAGEBUFFER_MAX];
} IMAGEBUFFER;

void imageBuffer_Store(IMAGEBUFFER *imageBuffer, uint32_t word);
void imageBuffer_Reset(IMAGEBUFFER *imageBuffer, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
IMAGEBUFFER *imageBuffer_Create();
void imageBuffer_Destroy(IMAGEBUFFER *imageBuffer);

#endif