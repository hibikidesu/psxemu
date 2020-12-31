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
			log_Error("Unknown Expansion 2 Offset 0x%X", offset);
			exit(1);
			break;
	}
}
