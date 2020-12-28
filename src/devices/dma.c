#include <stdlib.h>
#include "dma.h"

void dma_Reset(DMA *dma) {
	dma->CONTROL = 0x07654321;
}

DMA *dma_Create() {
	DMA *dma = malloc(sizeof(DMA));
	dma_Reset(dma);
	return dma;
}

void dma_Destroy(DMA *dma) {
	free(dma);
}
