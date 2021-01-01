#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ram.h"
#include "../utils/utils.h"
#include "../utils/logger.h"
#include "../cpu/instructions.h"

ExeFile *ram_LoadEXE(RAM *ram, char *path) {
	char buffer[0x10];
	uint32_t length = 0;
	FILE *file = fopen(path, "rb");
	if (file == NULL) {
		log_Error("Failed to load EXE, file not found.");
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Check header
	fread(buffer, 1, 0x10, file);
	if (strncmp(buffer, "PS-X EXE", 0x10) != 0) {
		log_Error("Failed to load EXE, invalid header.");
		fclose(file);
		return NULL;
	}

	ExeFile *exe = malloc(sizeof(ExeFile));
	// Initial SP
	fread(buffer, 1, sizeof(uint32_t), file);
	exe->initial_pc = utils_LoadLittleEndianInt((uint8_t*)buffer, 0);
	// Initial GP
	fread(buffer, 1, sizeof(uint32_t), file);
	exe->initial_gp = utils_LoadLittleEndianInt((uint8_t*)buffer, 0);
	// Destination in RAM
	fread(buffer, 1, sizeof(uint32_t), file);
	exe->ram_destination = utils_LoadLittleEndianInt((uint8_t*)buffer, 0);
	// Filesize
	fread(buffer, 1, sizeof(uint32_t), file);
	exe->file_size = utils_LoadLittleEndianInt((uint8_t*)buffer, 0);

	// Unused
	fseek(file, 8, SEEK_SET);

	// Memfill start address
	fread(buffer, 1, sizeof(uint32_t), file);
	exe->fill_start_address = utils_LoadLittleEndianInt((uint8_t*)buffer, 0);
	// Memfill size
	fread(buffer, 1, sizeof(uint32_t), file);
	exe->fill_size = utils_LoadLittleEndianInt((uint8_t*)buffer, 0);
	// Initial SP & FP base
	fread(buffer, 1, sizeof(uint32_t), file);
	exe->initial_spfp_base = utils_LoadLittleEndianInt((uint8_t*)buffer, 0);
	// SP FP off
	fread(buffer, 1, sizeof(uint32_t), file);
	exe->initial_spfp_off = utils_LoadLittleEndianInt((uint8_t*)buffer, 0);

	// Marker unchecked, TODO?

	fseek(file, 0x800, SEEK_SET);
	fread(ram->data + mask_region(exe->ram_destination), 1, length - 0x800, file);

	return exe;
}

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
