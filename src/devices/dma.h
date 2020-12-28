#ifndef PSX_DMA
#define PSX_DMA

#include <stdint.h>
#include <stdbool.h>
#include "channel.h"

// Memory Offsets
#define DMA_OFFSET 0x1f801080
#define DMA_SIZE 0x80

typedef struct {
	// 7 Channels
	CHANNEL *MDECin;
	CHANNEL *MDECout;
	CHANNEL *GPU;
	CHANNEL *CDROM;
	CHANNEL *SPU;
	CHANNEL *PIO;
	CHANNEL *OTC;
	// Registers
	uint32_t CONTROL;
	// Interrupt
	bool irq_enable;
	uint8_t irq_channel_enable;
	uint8_t irq_channel_flags;
	bool irq_force;
	uint8_t irq_dummy;
} DMA;

void dma_SetControl(DMA *dma, uint32_t value);
void dma_DumpChannels(DMA *dma);
CHANNEL *dma_GetChannelFromIndex(DMA *dma, uint8_t channel);
bool dma_GetIRQ(DMA *dma);
uint32_t dma_GetInterrupt(DMA *dma);
void dma_SetInterrupt(DMA *dma, uint32_t value);
uint32_t dma_ReadRegister(DMA *dma, uint32_t offset);
void dma_Reset(DMA *dma);
DMA *dma_Create();
void dma_Destroy(DMA *dma);

#endif