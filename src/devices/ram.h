#ifndef PSX_RAM
#define PSX_RAM

#include <stdint.h>

#define RAM_OFFSET 0
#define RAM_SIZE 2 * 1024 * 1024

typedef struct {
	uint8_t data[RAM_SIZE];
} RAM;

uint8_t ram_Load32(RAM *ram, uint32_t address);
void ram_Store32(RAM *ram, uint32_t offset, uint32_t value);
RAM *ram_Create();
void ram_Destroy(RAM *ram);

#endif