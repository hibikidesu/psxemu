#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "bios.h"
#include "../utils/utils.h"
#include "../utils/logger.h"

uint32_t bios_Load32(BIOS *bios, uint32_t address) {
	if ((address % 4) != 0) {
		log_Error("Unaligned Read of BIOS 0x%X", address);
		exit(1);
	} 

	// Check if is in range then return data from address if is.
	if (utils_AddressInRange(address, BIOS_OFFSET, BIOS_SIZE)) {
		return utils_LoadLittleEndian(bios->data, address - BIOS_OFFSET);
	}

	log_Error("ADDRESS 0x%X NOT IN RANGE OF BIOS!!!", address);
	exit(1);
	return 0;
}

int bios_LoadBios(BIOS *bios, char *biosFile) {
	FILE *file = NULL;
	size_t length;

	// Open file
	file = fopen(biosFile, "rb");

	// Check if file exists
	if (file == NULL) {
		log_Error("Invalid BIOS file.");
		return 1;
	}

	// Read length
	fseek(file, 0L, SEEK_END);
	length = ftell(file);
	fseek(file, 0L, SEEK_SET);
	log_Debug("BIOS File Size: %d/%d", length, BIOS_SIZE);

	// Check length valid
	if (length != BIOS_SIZE) {
		log_Error("Invalid BIOS length");
		fclose(file);
		return 1;
	}

	// Read BIOS into buffer
	fread(bios->data, length, 1, file);
	log_Info("Successfully Read BIOS");

	// Close
	fclose(file);
	return 0;
}

BIOS *bios_Create(char *biosFile) {
	// Allocate BIOS
	BIOS *bios = malloc(sizeof(BIOS));

	// Load BIOS file into BIOS struct
	if (bios_LoadBios(bios, biosFile)) {
		free(bios);
		return NULL;
	}

	// Return BIOS
	return bios;
}

void bios_Destroy(BIOS *bios) {
	free(bios);
}
