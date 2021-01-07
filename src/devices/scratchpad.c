#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "scratchpad.h"
#include "../utils/logger.h"
#include "../utils/utils.h"

uint32_t scratchpad_LoadInt(SCRATCHPAD *scratchpad, uint32_t address) {
	// Check if is in range then return data from address if is.
	if (utils_AddressInRange(address, SCRATCHPAD_OFFSET, SCRATCHPAD_SIZE)) {
		return utils_LoadLittleEndianInt(scratchpad->data, address - SCRATCHPAD_OFFSET);
	}

	log_Error("%s ADDRESS 0x%X NOT IN RANGE OF SCRATCPAD!!!", __FUNCTION__, address);
	exit(1);
	return 0;
}

uint16_t scratchpad_LoadShort(SCRATCHPAD *scratchpad, uint32_t address) {
	// Check if is in range then return data from address if is.
	if (utils_AddressInRange(address, SCRATCHPAD_OFFSET, SCRATCHPAD_SIZE)) {
		return utils_LoadLittleEndianShort(scratchpad->data, address - SCRATCHPAD_OFFSET);
	}

	log_Error("%s ADDRESS 0x%X NOT IN RANGE OF SCRATCPAD!!!", __FUNCTION__, address);
	exit(1);
	return 0;
}

uint8_t scratchpad_LoadByte(SCRATCHPAD *scratchpad, uint32_t address) {
	// Check if is in range then return data from address if is.
	if (utils_AddressInRange(address, SCRATCHPAD_OFFSET, SCRATCHPAD_SIZE)) {
		return utils_LoadLittleEndianByte(scratchpad->data, address - SCRATCHPAD_OFFSET);
	}

	log_Error("%s ADDRESS 0x%X NOT IN RANGE OF SCRATCPAD!!!", __FUNCTION__, address);
	exit(1);
	return 0;
}

void scratchpad_StoreInt(SCRATCHPAD *scratchpad, uint32_t address, uint32_t value) {
	if (utils_AddressInRange(address, SCRATCHPAD_OFFSET, SCRATCHPAD_SIZE)) {
		return utils_StoreLittleEndianInt(scratchpad->data, address - SCRATCHPAD_OFFSET, value);
	}
	log_Error("%s ADDRESS 0x%X NOT IN RANGE OF SCRATCHPAD!!!", __FUNCTION__, address);
	exit(1);
}

void scratchpad_StoreShort(SCRATCHPAD *scratchpad, uint32_t address, uint16_t value) {
	if (utils_AddressInRange(address, SCRATCHPAD_OFFSET, SCRATCHPAD_SIZE)) {
		return utils_StoreLittleEndianShort(scratchpad->data, address - SCRATCHPAD_OFFSET, value);
	}
	log_Error("%s ADDRESS 0x%X NOT IN RANGE OF SCRATCHPAD!!!", __FUNCTION__, address);
	exit(1);
}

void scratchpad_StoreByte(SCRATCHPAD *scratchpad, uint32_t address, uint8_t value) {
	if (utils_AddressInRange(address, SCRATCHPAD_OFFSET, SCRATCHPAD_SIZE)) {
		return utils_StoreLittleEndianByte(scratchpad->data, address - SCRATCHPAD_OFFSET, value);
	}
	log_Error("%s ADDRESS 0x%X NOT IN RANGE OF SCRATCHPAD!!!", __FUNCTION__, address);
	exit(1);
}

void scratchpad_Reset(SCRATCHPAD *scratchpad) {
	memset(scratchpad->data, 0, sizeof(scratchpad->data));
}

SCRATCHPAD *scratchpad_Create() {
	SCRATCHPAD *scratchpad = malloc(sizeof(SCRATCHPAD));
	return scratchpad;
}

void scratchpad_Destroy(SCRATCHPAD *scratchpad) {
	free(scratchpad);
}
