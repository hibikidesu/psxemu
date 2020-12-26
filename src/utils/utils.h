#ifndef PSX_UTILS
#define PSX_UTILS

#include <stdint.h>
#include <stdbool.h>

bool utils_AddressInRange(uint32_t address, uint32_t location, uint32_t size);
uint8_t utils_LoadLittleEndianByte(uint8_t *data, uint32_t offset);
uint32_t utils_LoadLittleEndianInt(uint8_t *data, uint32_t offset);
void utils_StoreLittleEndianByte(uint8_t *data, uint32_t offset, uint8_t value);
void utils_StoreLittleEndianInt(uint8_t *data, uint32_t offset, uint32_t value);

#endif