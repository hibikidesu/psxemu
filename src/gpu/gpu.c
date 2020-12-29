#include <stdlib.h>
#include <stdbool.h>
#include "gpu.h"
#include "../utils/logger.h"

uint32_t gpu_GetStatus(GPU *gpu) {
	uint32_t r = 0;
	r |= ((uint32_t)gpu->page_base_x) << 0;
	r |= ((uint32_t)gpu->page_base_y) << 4;
	r |= ((uint32_t)gpu->semi_transparancy) << 5;
	r |= ((uint32_t)gpu->texture_depth) << 7;
	r |= ((uint32_t)gpu->dithering) << 9;
	r |= ((uint32_t)gpu->draw_to_display) << 10;
	r |= ((uint32_t)gpu->force_set_mask_bit) << 11;
	r |= ((uint32_t)gpu->preserve_masked_pixels) << 12;
	r |= ((uint32_t)gpu->field) << 13;
	// Bit 14 not supported
	r |= ((uint32_t)gpu->texture_disabled) << 15;
	r |= gpu_IntoStatus(gpu);
	r |= ((uint32_t)gpu->vres) << 19;
	r |= ((uint32_t)gpu->vmode) << 20;
	r |= ((uint32_t)gpu->display_depth) << 21;
	r |= ((uint32_t)gpu->interlaced) << 22;
	r |= ((uint32_t)gpu->display_disabled) << 23;
	r |= ((uint32_t)gpu->interrupt) << 24;

	// CPU Always Ready
	r |= (1 << 26);
	// Ready to send VRAM to CPU
	r |= (1 << 27);
	// Ready to recieve DMA block
	r |= (1 << 28);

	r |= ((uint32_t)gpu->dma_direction) << 29;

	// Bit 31 = Currently drawn the line
	r |= (0 << 31);

	switch (gpu->dma_direction) {
		case DirOff:
			r |= (0 << 25);
			break;
		case DirFifo:
			r |= (1 << 25);
			break;
		case DirCPUtoGP0:
			r |= (((r >> 28) & 1) << 25);
			break;
		case DirVRAMtoCPU:
			r |= (((r >> 27) & 1) << 25);
			break;
		default:
			r |= (0 << 25);
			break;
	}

	return r;
}

uint32_t gpu_IntoStatus(GPU *gpu) {
	return ((uint32_t)gpu->hres) << 16;
}

uint8_t gpu_CreateHRES(uint8_t hr1, uint8_t hr2) {
	return (hr2 & 1) | ((hr1 & 3) << 1);
}

void gpu_Reset(GPU *gpu) {
	gpu->interrupt = false;

	gpu->page_base_x = 0;
	gpu->page_base_y = 0;
	gpu->semi_transparancy = 0;
	gpu->texture_depth = t4bit;
	gpu->texture_window_x_mask = 0;
	gpu->texture_window_y_mask = 0;
	gpu->texture_window_x_offset = 0;
	gpu->texture_window_y_offset = 0;
	gpu->dithering = false;
	gpu->draw_to_display = false;
	gpu->texture_disabled = false;
	gpu->rectangle_t_x_flip = false;
	gpu->rectangle_t_y_flip = false;
	gpu->drawing_area_left = 0;
	gpu->drawing_area_top = 0;
	gpu->drawing_area_right = 0;
	gpu->drawing_area_bottom = 0;
	gpu->drawing_x_offset = 0;
	gpu->drawing_y_offset = 0;
	gpu->force_set_mask_bit = false;
	gpu->preserve_masked_pixels = false;

	gpu->dma_direction = DirOff;
	
	gpu->display_disabled = true;
	gpu->display_vram_x_start = 0;
	gpu->display_vram_y_start = 0;
	gpu->hres = gpu_CreateHRES(0, 0);
	gpu->vres = Y240Lines;

	gpu->vmode = NTSC;
	gpu->interlaced = false;
	gpu->display_horiz_start = 0x200;
	gpu->display_horiz_end = 0xc00;
	gpu->display_line_start = 0x10;
	gpu->display_line_end = 0x100;
	gpu->display_depth = D15Bits;

	// Clear FIFO wen implement
}

GPU *gpu_Create() {
	GPU *gpu = malloc(sizeof(GPU));
	gpu->field = fieldTop;	
	gpu->instruction = 0;
	gpu_Reset(gpu);
	return gpu;
}

void gpu_Destroy(GPU *gpu) {
	free(gpu);
}

//
// GP0
//

void gp0_drawMode(GPU *gpu) {
	gpu->page_base_x = (uint8_t)(gpu->instruction & 0xf);
	gpu->page_base_y = (uint8_t)((gpu->instruction >> 4) & 1);
	gpu->semi_transparancy = (uint8_t)((gpu->instruction >> 5) & 3);
	
	switch ((gpu->instruction >> 7) & 3) {
		case 0:
			gpu->texture_depth = t4bit;
			break;
		case 1:
			gpu->texture_depth = t8bit;
			break;
		case 2:
			gpu->texture_depth = t15bit;
			break;
		default:
			log_Error("%s Unknown Depth 0x%X", (gpu->instruction >> 7) & 3);
			exit(1);
			break;
	}

	gpu->dithering = ((gpu->instruction >> 9) & 1) != 0;
	gpu->draw_to_display = ((gpu->instruction >> 10) & 1) != 0;
	gpu->texture_disabled = ((gpu->instruction >> 11) & 1) != 0;
	gpu->rectangle_t_x_flip = ((gpu->instruction >> 12) & 1) != 0;
	gpu->rectangle_t_y_flip = ((gpu->instruction >> 13) & 1) != 0;
}

void gpu_HandleGP0(GPU *gpu, uint32_t value) {
	gpu->instruction = (value >> 24) & 0xff;
	switch (gpu->instruction) {
		case GP0_NOP:
			break;
		case GP0_DRAWMODE:
			gp0_drawMode(gpu);
			break;
		default:
			log_Error("Unknown GP0 opcode 0x%X, 0x%X", gpu->instruction, value);
			exit(1);
			break;
	}
}

//
// GP1
//

void gpu_HandleGP1(GPU *gpu, uint32_t value) {
	gpu->instruction = (value >> 24) & 0xff;
	switch (gpu->instruction) {
		default:
			log_Error("Unknown GP1 opcode 0x%X, 0x%X", gpu->instruction, value);
			exit(1);
			break;
	}
}