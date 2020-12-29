#include <stdlib.h>
#include <stdbool.h>
#include "gpu.h"

uint32_t gpu_GetStatus(GPU *gpu) {
	uint32_t r = 0;
	return r;
}

uint8_t gpu_CreateHRES(uint8_t hr1, uint8_t hr2) {
	return (hr2 & 1) | ((hr1 & 3) << 1);
}

void gpu_Reset(GPU *gpu) {
	gpu->page_base_x = 0;
	gpu->page_base_y = 0;
	gpu->semi_transparancy = 0;
	gpu->texture_depth = t4bit;
	gpu->dithering = false;
	gpu->draw_to_display = false;
	gpu->force_set_mask_bit = false;
	gpu->preserve_masked_pixels = false;
	gpu->field = fieldTop;
	gpu->texture_disabled = false;
	gpu->hres = gpu_CreateHRES(0, 0);
	gpu->vres = Y240Lines;
	gpu->vmode = NTSC;
	gpu->display_depth = D15Bits;
	gpu->interlaced = false;
	gpu->display_disabled = true;
	gpu->interrupt = false;
	gpu->dma_direction = 0;
}

GPU *gpu_Create() {
	GPU *gpu = malloc(sizeof(GPU));
	gpu_Reset(gpu);
	return gpu;
}

void gpu_Destroy(GPU *gpu) {
	free(gpu);
}