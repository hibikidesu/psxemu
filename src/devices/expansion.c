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

void expansion1_StoreInt(CPU *cpu, uint32_t offset, uint32_t value) {
	switch (offset) {
		// If its a bad expansion
		case 0x1f802000:
		case 0x1f000000:
			log_Error("Bad expansion: Address 0x%X", offset);
			exit(1);
			break;
		default:
			log_Error("%s Unknown Expansion 1 Offset 0x%X", __FUNCTION__, offset);
			exit(1);
			break;
	}
}
