#ifndef PSX_GPU
#define PSX_GPU

#include <stdint.h>
#include <stdbool.h>
#include "commandbuffer.h"
#include "imagebuffer.h"

// GP0 Instructions
#define GP0_NOP					0x00
#define GP0_CLEARCACHE			0x01
#define GP0_FILLRECT			0x02
#define GP0_UNKNOWN				0x08
#define GP0_QUADMONOOPAQUE		0x28
#define GP0_QUADTEXBLENDOPAQUE	0x2c
#define GP0_TRISHADEOPAQUE		0x30
#define GP0_QUADSHADEOPAQUE		0x38
#define GP0_RECT16STTEX			0x7F
#define GP0_IMAGE_LOAD			0xA0
#define GP0_IMAGE_STORE			0xC0
#define GP0_DRAWMODE			0xE1
#define GP0_SETTEXWIN			0xE2
#define GP0_SETDRAWTL			0xE3
#define GP0_SETDRAWBR			0xE4
#define GP0_SETDRAWOFFSET		0xE5
#define GP0_MASKBITSETTING		0xE6

// GP1 Instructions
#define GP1_RESET			0x00
#define GP1_RESETCMDBUFFER	0x01
#define GP1_ACK_IRQ			0x02
#define GP1_ENABLE_DISPLAY	0x03
#define GP1_DMADIR			0x04
#define GP1_VRAM_START		0x05
#define GP1_DISPLAY_HRANGE	0x06
#define GP1_DISPLAY_VRANGE	0x07
#define GP1_DISPLAYMODE		0x08

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
typedef enum {
	DirOff = 0,
	DirFifo = 1,
	DirCPUtoGP0 = 2,
	DirVRAMtoCPU = 3
} GPUDMADirection;
typedef enum {
	// Handling commands
	Command,
	// Loading images into VRAM
	ImageLoad
} GP0Mode;

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
	GPUDMADirection dma_direction;
	// Current instruction
	uint32_t instruction;
	// Used for mirroring textures?
	bool rectangle_t_x_flip;
	bool rectangle_t_y_flip;
	// Texture window x mask
	uint8_t texture_window_x_mask;
	// Texture window y mask
	uint8_t texture_window_y_mask;
	// Texture window x offset
	uint8_t texture_window_x_offset;
	// Texture window y offset
	uint8_t texture_window_y_offset;
	// Left most drawing area
	uint16_t drawing_area_left;
	// Top most drawing area
	uint16_t drawing_area_top;
	// Right most drawing area
	uint16_t drawing_area_right;
	// Bottom most drawing area
	uint16_t drawing_area_bottom;
	// Horizontal drawing offset applied to all vertex
	int16_t drawing_x_offset;
	// Vertical drawing offset applied to all vertex
	int16_t drawing_y_offset;
	// First column of display area in VRAM
	uint16_t display_vram_x_start;
	// First line of the display area in VRAM
	uint16_t display_vram_y_start;
	// Display output horizontal start relative to HSYNC
	uint16_t display_horiz_start;
	// Display output horizontal end relative to HSYNC
	uint16_t display_horiz_end;
	// Display line first relative to VSYNC
	uint16_t display_line_start;
	// Display line last relative to VSYNC
	uint16_t display_line_end;
	// Contains current GP0 command
	CommandBuffer *gp0_cmd;
	// Remaining words for current GP0 command
	uint32_t gp0_words_remaining;
	// Instruction to run
	uint32_t gp0_ins;
	GP0Mode gp0_mode;
	// Image Buffer
	IMAGEBUFFER *imageBuffer;
} GPU;

uint32_t gpu_Load32(GPU *gpu, uint32_t offset);
uint32_t gpu_GetStatus(GPU *gpu);
uint32_t gpu_IntoStatus(GPU *gpu);
uint32_t gpu_GetStatus(GPU *gpu);
uint8_t gpu_CreateHRES(uint8_t hr1, uint8_t hr2);
void gpu_Reset(GPU *gpu);
GPU *gpu_Create();
void gpu_Destroy(GPU *gpu);
void gpu_HandleGP0(GPU *gpu, uint32_t value);
void gpu_HandleGP1(GPU *gpu, uint32_t value);

#endif