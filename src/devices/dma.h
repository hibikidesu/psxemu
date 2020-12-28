#ifndef PSX_DMA
#define PSX_DMA

#include <stdint.h>

#define DMA_OFFSET 0x1f801080
#define DMA_SIZE 0x80

typedef struct {
	uint8_t MDECin[0x10];
	uint8_t MDECout[0x10];
	uint8_t GPU[0x10];
	uint8_t CDROM[0x10];
	uint8_t SPU[0x10];
	uint8_t PIO[0x10];
	uint8_t OTC[0x10];
	uint32_t CONTROL;
	uint32_t INTERRUPT;
} DMA;

void dma_Reset(DMA *dma);
DMA *dma_Create();
void dma_Destroy(DMA *dma);

#endif