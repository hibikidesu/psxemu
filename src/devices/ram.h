#ifndef PSX_RAM
#define PSX_RAM

#include <stdint.h>

// KSEG1
#define RAM_OFFSET 0x0
#define RAM_SIZE 2 * 1024 * 1024

typedef struct {
	uint8_t data[RAM_SIZE];
} RAM;

void ram_Dump(RAM *ram, char *path);
uint8_t ram_LoadByte(RAM *ram, uint32_t address);
uint32_t ram_LoadInt(RAM *ram, uint32_t address);
void ram_StoreByte(RAM *ram, uint32_t address, uint8_t value);
void ram_StoreInt(RAM *ram, uint32_t offset, uint32_t value);
void ram_Reset(RAM *ram);
RAM *ram_Create();
void ram_Destroy(RAM *ram);

#endif