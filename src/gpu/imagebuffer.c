#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "imagebuffer.h"

void imageBuffer_Store(IMAGEBUFFER *imageBuffer, uint32_t word) {
	// Assert if going to overflow
	assert(imageBuffer->index + 2 <= IMAGEBUFFER_MAX);

	imageBuffer->buffer[imageBuffer->index] = (uint16_t)word;
	imageBuffer->index += 1;
	imageBuffer->buffer[imageBuffer->index] = (uint16_t)(word >> 16);
	imageBuffer->index += 1;
}

void imageBuffer_Reset(IMAGEBUFFER *imageBuffer, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
	imageBuffer->x = x;
	imageBuffer->y = y;
	imageBuffer->w = w;
	imageBuffer->h = h;
	imageBuffer->index = (x + 1) * (y + 1);
}

IMAGEBUFFER *imageBuffer_Create() {
	IMAGEBUFFER *imageBuffer = malloc(sizeof(IMAGEBUFFER));
	imageBuffer_Reset(imageBuffer, 0, 0, 0, 0);
	return imageBuffer;
}

void imageBuffer_Destroy(IMAGEBUFFER *imageBuffer) {
	free(imageBuffer);
}