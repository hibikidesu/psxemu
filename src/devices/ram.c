#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ram.h"
#include "../utils/utils.h"
#include "../utils/logger.h"

void ram_Dump(RAM *ram, char *path) {
	FILE *file = NULL;

	file = fopen(path, "wb");
	if (file == NULL) {
		log_Error("Failed to open path for dumping memory to.");
		return;
	}

	fwrite(ram->data, 1, sizeof(ram->data), file);
	fclose(file);
}

uint8_t ram_LoadByte(RAM *ram, uint32_t address) {
	// Check if is in range then return data from address if is.
	if (utils_AddressInRange(address, RAM_OFFSET, RAM_SIZE)) {
		return utils_LoadLittleEndianByte(ram->data, address - RAM_OFFSET);
	}

	log_Error("%s ADDRESS 0x%X NOT IN RANGE OF RAM!!!", __FUNCTION__, address);
	exit(1);
	return 0;
}

uint16_t ram_LoadShort(RAM *ram, uint32_t address) {
	// Check if is in range then return data from address if is.
	if (utils_AddressInRange(address, RAM_OFFSET, RAM_SIZE)) {
		return utils_LoadLittleEndianShort(ram->data, address - RAM_OFFSET);
	}

	log_Error("%s ADDRESS 0x%X NOT IN RANGE OF RAM!!!", __FUNCTION__, address);
	exit(1);
	return 0;
}

uint32_t ram_LoadInt(RAM *ram, uint32_t address) {
	if ((address % 4) != 0) {
		log_Error("Unaligned Read of RAM 0x%X", address);
		exit(1);
	} 

	// Check if is in range then return data from address if is.
	if (utils_AddressInRange(address, RAM_OFFSET, RAM_SIZE)) {
		return utils_LoadLittleEndianInt(ram->data, address - RAM_OFFSET);
	}

	log_Error("%s ADDRESS 0x%X NOT IN RANGE OF RAM!!!", __FUNCTION__, address);
	exit(1);
	return 0;
}

void ram_StoreByte(RAM *ram, uint32_t address, uint8_t value) {
	if (utils_AddressInRange(address, RAM_OFFSET, RAM_SIZE)) {
		return utils_StoreLittleEndianByte(ram->data, address - RAM_OFFSET, value);
	}
	log_Error("%s ADDRESS 0x%X NOT IN RANGE OF RAM!!!", __FUNCTION__, address);
	exit(1);
}

void ram_StoreShort(RAM *ram, uint32_t address, uint32_t value) {
	if (utils_AddressInRange(address, RAM_OFFSET, RAM_SIZE)) {
		return utils_StoreLittleEndianShort(ram->data, address - RAM_OFFSET, value);
	}
	log_Error("%s ADDRESS 0x%X NOT IN RANGE OF RAM!!!", __FUNCTION__, address);
	exit(1);
}

void ram_StoreInt(RAM *ram, uint32_t address, uint32_t value) {
	if (utils_AddressInRange(address, RAM_OFFSET, RAM_SIZE)) {
		return utils_StoreLittleEndianInt(ram->data, address - RAM_OFFSET, value);
	}
	log_Error("%s ADDRESS 0x%X NOT IN RANGE OF RAM!!!", __FUNCTION__, address);
	exit(1);
}

void ram_Reset(RAM *ram) {
	memset(ram->data, 0, sizeof(ram->data));
}

RAM *ram_Create() {
	RAM *ram = malloc(sizeof(RAM));
	ram_Reset(ram);
	return ram;
}

void ram_Destroy(RAM *ram) {
	free(ram);
}
