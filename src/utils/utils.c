#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <byteswap.h>
#include "utils.h"
#include "../utils/logger.h"

bool utils_AddressInRange(uint32_t address, uint32_t location, uint32_t size) {
	if (address >= location && address < (location + size)) {
		return true;
	}
	return false;
}

uint8_t utils_LoadLittleEndianByte(uint8_t *data, uint32_t offset) {
	return data[offset];
}

uint16_t utils_LoadLittleEndianShort(uint8_t *data, uint32_t offset) {
	uint16_t b0 = data[offset + 0];
	uint16_t b1 = data[offset + 1];
	return b0 | (b1 << 8);
}

uint32_t utils_LoadLittleEndianInt(uint8_t *data, uint32_t offset) {
	uint8_t b0, b1, b2, b3;
	// Convert to little endian
	b0 = data[offset + 0];
	b1 = data[offset + 1];
	b2 = data[offset + 2];
	b3 = data[offset + 3];
	return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}

void utils_StoreLittleEndianByte(uint8_t *data, uint32_t offset, uint8_t value) {
	data[offset] = value;
}

void utils_StoreLittleEndianShort(uint8_t *data, uint32_t offset, uint16_t value) {
	data[offset + 0] = (uint8_t)value;
	data[offset + 1] = (uint8_t)(value >> 8);
}

void utils_StoreLittleEndianInt(uint8_t *data, uint32_t offset, uint32_t value) {
	data[offset + 0] = (uint8_t)value;
	data[offset + 1] = (uint8_t)(value >> 8);
	data[offset + 2] = (uint8_t)(value >> 16);
	data[offset + 3] = (uint8_t)(value >> 24);
}
