#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "dma.h"
#include "channel.h"
#include "devices.h"
#include "../utils/logger.h"

void dma_DumpChannels(DMA *dma) {
	int i;
	CHANNEL *channel = NULL;
	for (i = 0; i < 7; i++) {
		channel = dma_GetChannelFromIndex(dma, i);
		log_Info("CHANNEL %d: 0x%X", i, channel_GetControl(channel));
	}
}

CHANNEL *dma_GetChannelFromIndex(DMA *dma, uint8_t index) {
	CHANNEL *channel = NULL;
	switch (index) {
		case 0:
			channel = dma->MDECin;
			break;
		case 1:
			channel = dma->MDECout;
			break;
		case 2:
			channel = dma->GPU;
			break;
		case 3:
			channel = dma->CDROM;
			break;
		case 4:
			channel = dma->SPU;
			break;
		case 5:
			channel = dma->PIO;
			break;
		case 6:
			channel = dma->OTC;
			break;
		default:
			log_Error("%s Unknown Channel 0x%X", __FUNCTION__, index);
			exit(1);
			break;
	}
	return channel;
}

void dma_SetControl(DMA *dma, uint32_t value) {
	dma->CONTROL = value;
}

bool dma_GetIRQ(DMA *dma) {
	return dma->irq_force || ((dma->irq_enable && (dma->irq_channel_flags & dma->irq_channel_enable)) != 0);
}

uint32_t dma_GetInterrupt(DMA *dma) {
	uint32_t r = 0;
	r |= (uint32_t)dma->irq_dummy;
	r |= ((uint32_t)dma->irq_force) << 15;
	r |= ((uint32_t)dma->irq_channel_enable) << 16;
	r |= ((uint32_t)dma->irq_enable) << 23;
	r |= ((uint32_t)dma->irq_channel_flags) << 24;
	r |= ((uint32_t)dma_GetIRQ(dma)) << 31;
	return r;
}

void dma_SetInterrupt(DMA *dma, uint32_t value) {
	dma->irq_dummy = (uint8_t)(value & 0x3f);
	dma->irq_force = ((value >> 15) & 1) != 0;
	dma->irq_channel_enable = (uint8_t)((value >> 16) & 0x7f);
	dma->irq_enable = ((value >> 23) & 1) != 0;

	// Reset if 1
	uint8_t ack = (uint8_t)((value >> 24) & 0x3f);
	dma->irq_channel_flags &= ~ack;
}

uint32_t dma_ReadRegister(DMA *dma, uint32_t offset) {
	uint32_t v;
	CHANNEL *channel = NULL;
	uint32_t major = (offset & 0x70) >> 4;
	uint32_t minor = offset & 0xf;

	// notlikebep
	switch (major) {
		// Channels
		case 0 ... 6:
			channel = dma_GetChannelFromIndex(dma, major);
			switch (minor) {
				// Base
				case 0:
					v = channel_GetBase(channel);
					break;
				// Get block
				case 4:
					v = channel_GetBlockControl(channel);
					break;
				// Return Control
				case 8:
					v = channel_GetControl(channel);
					break;
				default:
					log_Error("%s Unknown Offset 0x%X", __FUNCTION__, offset);
					exit(1);
					break;
			}
			break;

		// Common Registers
		case 7:
			switch (minor) {
				// Control
				case 0:
					v = dma->CONTROL;
					break;
				// Interrupt
				case 4:
					v = dma_GetInterrupt(dma);
					break;
				default:
					log_Error("%s Unknown Offset 0x%X", __FUNCTION__, offset);
					exit(1);
					break;
			}
			break;

		default:
			log_Error("%s Unknown Offset 0x%X", __FUNCTION__, offset);
			exit(1);
			break;
	}

	return v;
}

void dma_Reset(DMA *dma) {
	dma->CONTROL = 0x07654321;
}

DMA *dma_Create() {
	DMA *dma = malloc(sizeof(DMA));

	// Create channels
	dma->MDECin = malloc(sizeof(CHANNEL));
	dma->MDECout = malloc(sizeof(CHANNEL));
	dma->GPU = malloc(sizeof(CHANNEL));
	dma->CDROM = malloc(sizeof(CHANNEL));
	dma->SPU = malloc(sizeof(CHANNEL));
	dma->PIO = malloc(sizeof(CHANNEL));
	dma->OTC = malloc(sizeof(CHANNEL));

	// Reset
	dma_Reset(dma);
	return dma;
}

void dma_Destroy(DMA *dma) {
	// Free Channels
	free(dma->MDECin);
	free(dma->MDECout);
	free(dma->GPU);
	free(dma->CDROM);
	free(dma->SPU);
	free(dma->PIO);
	free(dma->OTC);

	// Free main struct
	free(dma);
}
