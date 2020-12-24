#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ram.h"
#include "utils.h"
#include "../utils/logger.h"

uint8_t ram_Load32(RAM *ram, uint32_t address) {
	if ((address % 4) != 0) {
		log_Error("Unaligned Read of RAM 0x%X", address);
		exit(1);
	} 

	// Check if is in range then return data from address if is.
	if (utils_AddressInRange(address, RAM_OFFSET, RAM_SIZE)) {
		return utils_LoadLittleEndian(ram->data, address);
	}

	log_Error("ADDRESS 0x%X NOT IN RANGE OF RAM!!!", address);
	exit(1);
	return 0;
}

void ram_Store32(RAM *ram, uint32_t address, uint32_t value) {
	if (utils_AddressInRange(address, RAM_OFFSET, RAM_SIZE)) {
		return utils_StoreLittleEndian(ram->data, address, value);
	}
	log_Error("ADDRESS 0x%X NOT IN RANGE OF RAM!!!", address);
	exit(1);
}

RAM *ram_Create() {
	RAM *ram = malloc(sizeof(RAM));
	memset(ram->data, 0xCA, sizeof(ram->data));
	return ram;
}

void ram_Destroy(RAM *ram) {
	free(ram);
}
