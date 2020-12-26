#ifndef PSX_BIOS
#define PSX_BIOS

#include <stdint.h>

// All BIOS's always have the same size.
// 512 * 1024
#define BIOS_SIZE 0x80000
#define BIOS_OFFSET 0x1fc00000

typedef struct {
	uint8_t data[BIOS_SIZE];
} BIOS;

uint8_t bios_LoadByte(BIOS *bios, uint32_t address);
uint32_t bios_LoadInt(BIOS *bios, uint32_t address);
int bios_LoadBios(BIOS *bios, char *biosFile);
BIOS *bios_Create(char *biosFile);
void bios_Destroy(BIOS *bios);

#endif