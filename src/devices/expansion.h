#ifndef PSX_EXPANSION
#define PSX_EXPANSION

#include "../cpu/cpu.h"

// Expansion 2 (I/O Ports)
#define EXPANSION_2_OFFSET 0x1F802000
#define EXPANSION_2_SIZE 66
#define EXPANSION_2_POST 0x1F802041

void expansion2_StoreByte(CPU *cpu, uint32_t offset, uint8_t value);

#endif