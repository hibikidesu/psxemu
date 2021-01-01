#ifndef PSX_RAM
#define PSX_RAM

#include <stdint.h>

// KSEG1
#define RAM_OFFSET 0x0
#define RAM_SIZE 2 * 1024 * 1024
#define RAM_BIOS_SIZE 64 * 1024  // 64K for bios

typedef struct {
	uint32_t initial_pc;
	uint32_t initial_gp;
	uint32_t ram_destination;
	uint32_t file_size;
	uint32_t fill_start_address;
	uint32_t fill_size;
	uint32_t initial_spfp_base;
	uint32_t initial_spfp_off;
	uint32_t marker;
} ExeFile;
typedef struct {
	uint8_t data[RAM_SIZE];
} RAM;

ExeFile *ram_LoadEXE(RAM *ram, char *path);
void ram_Dump(RAM *ram, char *path);
uint8_t ram_LoadByte(RAM *ram, uint32_t address);
uint16_t ram_LoadShort(RAM *ram, uint32_t address);
uint32_t ram_LoadInt(RAM *ram, uint32_t address);
void ram_StoreByte(RAM *ram, uint32_t address, uint8_t value);
void ram_StoreShort(RAM *ram, uint32_t address, uint32_t value);
void ram_StoreInt(RAM *ram, uint32_t offset, uint32_t value);
void ram_Reset(RAM *ram);
RAM *ram_Create();
void ram_Destroy(RAM *ram);

#endif