#include <stdlib.h>
#include "devices.h"
#include "bios.h"
#include "ram.h"
#include "dma.h"
#include "channel.h"
#include "../gpu/gpu.h"
#include "../utils/logger.h"

void devices_RunDMALinkedList(DEVICES *devices, CHANNEL *channel, uint8_t index) {
	uint32_t addr = 0;
	uint32_t header = 0;
	uint32_t size = 0;
	uint32_t command = 0;

	// Check if supported mode
	if (channel->direction == toRam) {
		log_Error("Invalid chanel direction toRam for linkedList.");
		exit(1);
	}

	// Linked list only allows the gpu channel?
	if (index != 2) {
		log_Error("Attempted to run linked lists on channel %u", index);
		exit(1);
	}

	addr = channel_GetBase(channel) & 0x1ffffc;

	while (1) {
		// High byte contains word count of packet
		header = ram_LoadInt(devices->ram, addr);
		size = header >> 24;

		while (size > 0) {
			addr = (addr + 4) & 0x1ffffc;
			command = ram_LoadInt(devices->ram, addr);
			gpu_HandleGP0(devices->gpu, command);
			size -= 1;
		}

		// Hardware checks size instead of 0xffffff?
		if ((header & 0x800000) != 0) {
			break;
		}

		addr = header & 0x1ffffc;
	}

	channel_SetDone(channel);
	log_Debug("Channel %u done", index);
}

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

	log_Debug("Channel %u Base Address: 0x%08X, Block Size: %u", index, addr, size);

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
						log_Error("Unimplemented channel block toram %u", index);
						exit(1);
						break;
				}
				ram_StoreInt(devices->ram, cur_addr, src_word);
				// log_Debug("Storing 0x%X in 0x%X from channel %d", src_word, cur_addr, index);
				break;
			case toDevice:
				src_word = ram_LoadInt(devices->ram, cur_addr);
				switch (index) {
					// GPU
					case 2:
						// log_Debug("RAM->GPU 0x%08X", src_word);
						gpu_HandleGP0(devices->gpu, src_word);
						break;
					default:
						log_Error("Unimplemented channel block todevice %u", index);
						exit(1);
						break;
				}
				break;
			default:
				break;
		}

		// Next
		addr += incr;
		size -= 1;
	}

	channel_SetDone(channel);
	log_Debug("Channel %u done", index);
}

void devices_RunDMA(DEVICES *devices, CHANNEL *channel, uint8_t index) {
	switch (channel->sync) {
		case linkedList:
			devices_RunDMALinkedList(devices, channel, index);
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
	log_Debug("DMA Set Register 0x%08X: 0x%08X", offset, value);

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
				log_Debug("Running Channel 0x%X", major);
				devices_RunDMA(devices, channel, major);
				log_Debug("Done 0x%X", major);
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

void devices_AddGPU(DEVICES *devices, GPU *gpu) {
	devices->gpu = gpu;
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
