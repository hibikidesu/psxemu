#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "expansion.h"
#include "../cpu/cpu.h"
#include "../utils/logger.h"

void expansion2_StoreByte(CPU *cpu, uint32_t offset, uint8_t value) {
	switch (offset) {
		case EXPANSION_2_POST:
			break;
		default:
			log_Error("%s Unknown Expansion 2 Offset 0x%X", __FUNCTION__, offset);
			exit(1);
			break;
	}
}

uint8_t expansion1_LoadByte(CPU *cpu, uint32_t offset) {
	uint8_t value = 0;
	switch (offset) {
		// Pre-Boot ID ("Licensed by Sony Computer Entertainment Inc.")
		// Not sure what to do with it, leaving blank
		case EXPANSION_1_PRE_BOOT_ID:
			break;
		default:
			log_Error("Unimplemented expansion 1 load int at 0x%X (0x%X)", offset, offset + EXPANSION_1_OFFSET);
			exit(1);
			break;
	}
	return value;
}

void expansion1_StoreInt(CPU *cpu, uint32_t offset, uint32_t value) {
	switch (offset) {
		// If its a bad expansion
		case 0x1f802000:
		case 0x1f000000:
			log_Error("Bad expansion: Address 0x%X", offset);
			exit(1);
			break;
		default:
			log_Error("%s Unknown Expansion 1 Offset 0x%X (0x%X)", __FUNCTION__, offset, offset + EXPANSION_1_OFFSET);
			exit(1);
			break;
	}
}
