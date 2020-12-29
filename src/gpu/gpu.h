#ifndef PSX_GPU
#define PSX_GPU

#include <stdint.h>
#include <stdbool.h>

// Bits per pixel
typedef enum {
	t4bit = 0,
	t8bit = 1,
	t15bit = 2
} GPUTextureDepth;

typedef enum {
	// Odd Lines
	fieldTop = 1,
	// Even lines
	fieldBotom = 0
} GPUField;

typedef enum {
	Y240Lines = 0,
	Y480Lines = 1
} GPUVRes;

typedef enum {
	NTSC = 0,
	PAL = 1
} GPUVideoMode;

typedef enum {
	D15Bits = 0,
	D24Bits = 1
} GPUDisplayDepth;

typedef struct {
	// Texture page X
	uint8_t page_base_x;
	// Texture page Y
	uint8_t page_base_y;
	// Describes how to blend source+destination?
	uint8_t semi_transparancy;
	// Texture color Depth
	GPUTextureDepth texture_depth;
	// Dithering from 24 to 15 bits RGB
	bool dithering;
	// Allow drawing to display area
	bool draw_to_display;
	// Force mask bit of pixel to 1 when writing to vram
	bool force_set_mask_bit;
	// Dont draw pixels with mask bit set
	bool preserve_masked_pixels;
	// Currently displayed field
	GPUField field;
	// Disable textures if true
	bool texture_disabled;
	// Video Horizontal Resolution
	uint8_t hres;
	// Video Vertical Resolution
	GPUVRes vres;
	// Video Mode
	GPUVideoMode vmode;
	// Display Depth
	GPUDisplayDepth display_depth;
	// Output interlaced instead of progressive
	bool interlaced;
	// Disable Display
	bool display_disabled;
	// When interrupt active
	bool interrupt;
	// DMA request direction
	int dma_direction;
} GPU;

uint32_t gpu_GetStatus(GPU *gpu);
uint8_t gpu_CreateHRES(uint8_t hr1, uint8_t hr2);
void gpu_Reset(GPU *gpu);
GPU *gpu_Create();
void gpu_Destroy(GPU *gpu);

#endif