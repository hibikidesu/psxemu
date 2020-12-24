#ifndef PSX_UTILS
#define PSX_UTILS

#include <stdint.h>
#include <stdbool.h>

bool utils_AddressInRange(uint32_t address, uint32_t location, uint32_t size);
uint32_t utils_LoadLittleEndian(uint8_t *data, uint32_t offset);
void utils_StoreLittleEndian(uint8_t *data, uint32_t offset, uint32_t value);

#endif