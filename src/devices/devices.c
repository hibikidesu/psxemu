#include <stdlib.h>
#include "devices.h"
#include "bios.h"
#include "ram.h"
#include "dma.h"
#include "channel.h"
#include "../utils/logger.h"

// Set DMA registers, Placed here and not in DMA to access other devices if needed.
void devices_DMASetRegister(DEVICES *devices, uint32_t offset, uint32_t value) {
	uint32_t major = (offset & 0x70) >> 4;
	uint32_t minor = offset & 0xf;
	CHANNEL *channel = NULL;

	// another notlikebep
	switch (major) {
		// Channels
		case 0 ... 6:
			channel = dma_GetChannelFromIndex(devices->dma, major);
			switch (minor) {
				// Set Base
				case 0:
					channel_SetBase(channel, value);
					break;
				// Set Block
				case 4:
					channel_SetBlockControl(channel, value);
				// Set Control
				case 8:
					channel_SetControl(channel, value);
					break;
				default:
					log_Error("%s Unknown Channel Offset 0x%X Value 0x%X", __FUNCTION__, offset, value);
					exit(1);
					break;
			}
			break;

		// Common Registers
		case 7:
			switch (minor) {
				// Control
				case 0:
					dma_SetControl(devices->dma, value);
					break;
				// Interrupt
				case 4:
					dma_SetInterrupt(devices->dma, value);
					break;
				default:
					log_Error("%s Unknown Common Offset 0x%X Value 0x%X", __FUNCTION__, offset, value);
					exit(1);
					break;
			}
			break;

		default:
			log_Error("%s Unknown Offset 0x%X Value 0x%X", __FUNCTION__, offset, value);
			exit(1);
			break;
	}
}

void devices_AddRAM(DEVICES *devices, RAM *ram) {
	devices->ram = ram;
}

void devices_AddBios(DEVICES *devices, BIOS *bios) {
	devices->bios = bios;
}

DEVICES *devices_Create() {
	DEVICES *devices = malloc(sizeof(DEVICES));
	devices->dma = dma_Create();
	return devices;
}

void devices_Destroy(DEVICES *devices) {
	dma_Destroy(devices->dma);
	free(devices);
}
