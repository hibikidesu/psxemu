#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "dma.h"
#include "channel.h"
#include "../utils/logger.h"

CHANNEL *dma_GetChannelFromIndex(DMA *dma, uint8_t channel) {
	CHANNEL *channel = NULL;
	switch (channel) {
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
			log_Error("%s Unknown Channel 0x%X", __FUNCTION__, channel);
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
	dma->irq_channel_flags &= !ack;
}

uint32_t dma_ReadRegister(DMA *dma, uint32_t offset) {
	uint32_t v;

	switch (offset - DMA_OFFSET) {
		case DMA_CONTROL:
			v = dma->CONTROL;
			break;
		case DMA_INTERRUPT:
			v = dma_GetInterrupt(dma);
			break;
		default:
			log_Debug("%s Unhandled Access 0x%X", __FUNCTION__, offset - DMA_OFFSET);
			exit(1);
			break;
	}

	return v;
}

void dma_SetRegister(DMA *dma, uint32_t offset, uint32_t value) {
	switch (offset - DMA_OFFSET) {
		case DMA_CONTROL:
			dma_SetControl(dma, value);
			break;
		case DMA_INTERRUPT:
			dma_SetInterrupt(dma, value);
			break;
		default:
			log_Debug("%s Unhandled Access 0x%X", __FUNCTION__, offset - DMA_OFFSET);
			exit(1);
			break;
	}
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
