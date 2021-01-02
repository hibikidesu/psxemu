#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "imagebuffer.h"
#include "../utils/logger.h"

void imageBuffer_IncrIndex(IMAGEBUFFER *imageBuffer) {
	imageBuffer->index += 1;
	imageBuffer->image_index += 1;
	if ((imageBuffer->image_index % imageBuffer->w) == 0) {
		imageBuffer->index = ((imageBuffer->y + (imageBuffer->image_index / imageBuffer->w)) * 1024) + imageBuffer->x;
	}
}

void imageBuffer_Store(IMAGEBUFFER *imageBuffer, uint32_t word) {
	// Assert if going to overflow
	assert(imageBuffer->index + 2 <= IMAGEBUFFER_MAX);

	imageBuffer->buffer[imageBuffer->index] = (uint16_t)word;
	imageBuffer_IncrIndex(imageBuffer);

	imageBuffer->buffer[imageBuffer->index] = (uint16_t)(word >> 16);
	imageBuffer_IncrIndex(imageBuffer);
}

void imageBuffer_Reset(IMAGEBUFFER *imageBuffer, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
	imageBuffer->x = x;
	imageBuffer->y = y;
	imageBuffer->w = w;
	imageBuffer->h = h;
	imageBuffer->image_index = 0;
	imageBuffer->index = x + y * 1024;
}

IMAGEBUFFER *imageBuffer_Create() {
	IMAGEBUFFER *imageBuffer = malloc(sizeof(IMAGEBUFFER));
	imageBuffer_Reset(imageBuffer, 0, 0, 0, 0);
	return imageBuffer;
}

void imageBuffer_Destroy(IMAGEBUFFER *imageBuffer) {
	free(imageBuffer);
}