#ifndef PSX_DEVICES
#define PSX_DEVICES

#include "bios.h"
#include "ram.h"
#include "dma.h"
#include "scratchpad.h"
#include "cdrom.h"
#include "../gpu/gpu.h"

typedef struct {
	BIOS *bios;
	RAM *ram;
	DMA *dma;
	GPU *gpu;
	SCRATCHPAD *scratchpad;
	CDROM *cdrom;
} DEVICES;

void devices_RunDMALinkedList(DEVICES *devices, CHANNEL *channel, uint8_t index);
void devices_RunDMABlock(DEVICES *devices, CHANNEL *channel, uint8_t index);
void devices_RunDMA(DEVICES *devices, CHANNEL *channel, uint8_t index);
void devices_DMASetRegister(DEVICES *devices, uint32_t offset, uint32_t value);
void devices_AddCDROM(DEVICES *devices, CDROM *cdrom);
void devices_AddGPU(DEVICES *devices, GPU *gpu);
void devices_AddRAM(DEVICES *devices, RAM *ram);
void devices_AddBios(DEVICES *devices, BIOS *bios);
DEVICES *devices_Create();
void devices_Destroy(DEVICES *devices);

#endif