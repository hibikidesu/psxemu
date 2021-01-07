#ifndef PSX_EXPANSION
#define PSX_EXPANSION

#include "../cpu/cpu.h"

// Expansion 1 (ROM/RAM)
#define EXPANSION_1_OFFSET 0x1f000000
#define EXPANSION_1_SIZE 8192 * 1024
#define EXPANSION_1_PRE_BOOT_ID 0x84

// Expansion 2 (I/O Ports)
#define EXPANSION_2_OFFSET 0x1F802000
#define EXPANSION_2_SIZE 66
#define EXPANSION_2_POST 0x1F802041

// Expansion 2
uint32_t expansion2_LoadInt(CPU *cpu, uint32_t offset);
void expansion2_StoreByte(CPU *cpu, uint32_t offset, uint8_t value);

// Expansion 1
uint8_t expansion1_LoadByte(CPU *cpu, uint32_t offset);
void expansion1_StoreInt(CPU *cpu, uint32_t offset, uint32_t value);

#endif