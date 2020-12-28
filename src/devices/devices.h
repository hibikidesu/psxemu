#ifndef PSX_DEVICES
#define PSX_DEVICES

#include "bios.h"
#include "ram.h"
#include "dma.h"

typedef struct {
	BIOS *bios;
	RAM *ram;
	DMA *dma;
} DEVICES;

void devices_AddRAM(DEVICES *devices, RAM *ram);
void devices_AddBios(DEVICES *devices, BIOS *bios);
DEVICES *devices_Create();
void devices_Destroy(DEVICES *devices);

#endif