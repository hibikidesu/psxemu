#include <stdlib.h>
#include "devices.h"
#include "bios.h"
#include "ram.h"
#include "dma.h"
#include "channel.h"
#include "../utils/logger.h"

// Block copy to next target
void devices_RunDMABlock(DEVICES *devices, CHANNEL *channel, uint8_t index) {
	uint32_t incr;
	uint32_t addr;
	uint32_t cur_addr;
	uint32_t size;
	uint32_t src_word;

	// Get incr count
	if (channel->step == increment) {
		incr = 4;
	} else {
		incr = -4;
	}

	// Get Address
	addr = channel_GetBase(channel);
	
	// Get transfer size
	if ((size = channel_GetTransferSize(channel)) == 0) {
		log_Error("Failed to find channel transfer size");
		exit(1);
	}

	// not again notlikebep
	while (size > 0) {
		cur_addr = addr & 0x1ffffc;
		switch (channel->direction) {
			case toRam:	
				// gah
				switch (index) {
					// OTC
					case 6:
						// imagine using ifs
						switch (size) {
							case 1:
								src_word = 0xffffff;
								break;
							default:
								src_word = (addr - 4) & 0x1fffff;
								break;
						}
						break;
					default:
						log_Error("Unimplemented channel block toram %d", index);
						exit(1);
						break;
				}
				ram_StoreInt(devices->ram, cur_addr, src_word);
				log_Debug("Storing 0x%X in 0x%X from channel %d", src_word, cur_addr, index);
				break;
			case toDevice:
				log_Error("toDevice block unimplemented");
				exit(1);
				break;
			default:
				break;
		}

		// Next
		addr += incr;
		size -= 1;
	}

	channel_SetDone(channel);
}

void devices_RunDMA(DEVICES *devices, CHANNEL *channel, uint8_t index) {
	switch (channel->sync) {
		case linkedList:
			log_Error("%s linked lists not supported", __FUNCTION__);
			exit(1);
			break;
		default:
			devices_RunDMABlock(devices, channel, index);
			break;
	}
}

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
			// Do dma if channel is active.
			if (channel_IsActive(channel)) {
				devices_RunDMA(devices, channel, major);
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
