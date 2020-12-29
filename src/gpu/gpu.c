#include <stdlib.h>
#include <stdbool.h>
#include "gpu.h"
#include "../utils/logger.h"

uint32_t gpu_Load32(GPU *gpu, uint32_t offset) {
	log_Debug("GPU Read at 0x%X", offset);
	return 0;
}

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

void gp0_SetDrawTL(GPU *gpu) {
	gpu->drawing_area_top = (uint16_t)((gpu->instruction >> 10) & 0x3ff);
	gpu->drawing_area_left = (uint16_t)(gpu->instruction &0x3ff);
}

void gp0_SetDrawBR(GPU *gpu) {
	gpu->drawing_area_bottom = (uint16_t)((gpu->instruction >> 10) & 0x3ff);
	gpu->drawing_area_right = (uint16_t)(gpu->instruction & 0x3ff);
}

void gp0_SetDrawingOffset(GPU *gpu) {
	uint16_t x = (uint16_t)(gpu->instruction & 0x7ff);
	uint16_t y = (uint16_t)((gpu->instruction >> 11) & 0x7ff);

	// Shift to force sign extension
	gpu->drawing_x_offset = ((int16_t)(x << 5)) >> 5;
	gpu->drawing_y_offset = ((int16_t)(y << 5)) >> 5;
}

void gp0_SetTextureWindow(GPU *gpu) {
	gpu->texture_window_x_mask = (uint8_t)(gpu->instruction & 0x1f);
	gpu->texture_window_y_mask = (uint8_t)((gpu->instruction >> 5) & 0x1f);
	gpu->texture_window_x_offset = (uint8_t)((gpu->instruction >> 10) & 0x1f);
	gpu->texture_window_y_offset = (uint8_t)((gpu->instruction >> 15) & 0x1f);
}

void gp0_MaskBitSetting(GPU *gpu) {
	gpu->force_set_mask_bit = (gpu->instruction & 1) != 0;
	gpu->preserve_masked_pixels = (gpu->instruction & 2) != 0;
}

void gpu_HandleGP0(GPU *gpu, uint32_t value) {
	gpu->instruction = (value >> 24) & 0xff;
	switch (gpu->instruction) {
		case GP0_NOP:
			break;
		case GP0_DRAWMODE:
			gp0_drawMode(gpu);
			break;
		case GP0_SETDRAWTL:
			gp0_SetDrawTL(gpu);
			break;
		case GP0_SETDRAWBR:
			gp0_SetDrawBR(gpu);
			break;
		case GP0_SETDRAWOFFSET:
			gp0_SetDrawingOffset(gpu);
			break;
		case GP0_SETTEXWIN:
			gp0_SetTextureWindow(gpu);
			break;
		case GP0_MASKBITSETTING:
			gp0_MaskBitSetting(gpu);
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

void gp1_DisplayMode(GPU *gpu) {
	uint8_t hr1 = (uint8_t)(gpu->instruction & 3);
	uint8_t hr2 = (uint8_t)((gpu->instruction >> 6) & 1);
	gpu->hres = gpu_CreateHRES(hr1, hr2);

	if ((gpu->instruction & 0x4) != 0) {
		gpu->vres = Y480Lines;
	} else {
		gpu->vres = Y240Lines;
	}

	if ((gpu->instruction & 0x8) != 0) {
		gpu->vmode = PAL;
	} else {
		gpu->vmode = NTSC;
	}

	if ((gpu->instruction & 0x10) != 0) {
		gpu->display_depth = D15Bits;
	} else {
		gpu->display_depth = D24Bits;
	}

	gpu->interlaced = (gpu->instruction & 0x20) != 0;
	if ((gpu->instruction & 0x80) != 0) {
		log_Error("Unsupported display mode 0x%X", gpu->instruction);
		exit(1);
	}
}

void gp1_DMADirectory(GPU *gpu) {
	switch (gpu->instruction & 3) {
		case 0:
			gpu->dma_direction = DirOff;
			break;
		case 1:
			gpu->dma_direction = DirFifo;
			break;
		case 2:
			gpu->dma_direction = DirCPUtoGP0;
			break;
		case 3:
			gpu->dma_direction = DirVRAMtoCPU;
			break;
		default:
			log_Error("Unknown %s value 0x%X", __FUNCTION__, gpu->instruction);
			break;
	}
}

void gp1_DisplayVRAMStart(GPU *gpu) {
	gpu->display_vram_x_start = (uint16_t)(gpu->instruction & 0x3fe);
	gpu->display_vram_y_start = (uint16_t)((gpu->instruction >> 10) & 0x1ff);
}

void gp1_DisplayHorizontalRange(GPU *gpu) {
	gpu->display_horiz_start = (uint16_t)(gpu->instruction & 0xfff);
	gpu->display_horiz_end = (uint16_t)((gpu->instruction >> 12) & 0xfff);
}

void gp1_DisplayVerticalRange(GPU *gpu) {
	gpu->display_line_start = (uint16_t)(gpu->instruction & 0x3ff);
	gpu->display_line_end = (uint16_t)((gpu->instruction >> 10) & 0x3ff);
}

void gpu_HandleGP1(GPU *gpu, uint32_t value) {
	gpu->instruction = (value >> 24) & 0xff;
	switch (gpu->instruction) {
		case GP1_RESET:
			gpu_Reset(gpu);
			break;
		case GP1_DISPLAYMODE:
			gp1_DisplayMode(gpu);
			break;
		case GP1_DMADIR:
			gp1_DMADirectory(gpu);
			break;
		case GP1_VRAM_START:
			gp1_DisplayVRAMStart(gpu);
			break;
		case GP1_DISPLAY_HRANGE:
			gp1_DisplayHorizontalRange(gpu);
			break;
		case GP1_DISPLAY_VRANGE:
			gp1_DisplayVerticalRange(gpu);
			break;
		default:
			log_Error("Unknown GP1 opcode 0x%X, 0x%X", gpu->instruction, value);
			exit(1);
			break;
	}
}