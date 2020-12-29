#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "expansion.h"
#include "../cpu/cpu.h"
#include "../utils/logger.h"

void handle_POST(CPU *cpu, uint8_t value) {
	switch (value) {
		default:
			log_Warn("Unhandled POSTcode 0x%X", value);
			break;
	}
}

void expansion2_StoreByte(CPU *cpu, uint32_t offset, uint8_t value) {
	switch (offset) {
		case EXPANSION_2_POST:
			handle_POST(cpu, value);
			break;
		default:
			log_Error("Unknown Expansion 2 Offset 0x%X", offset);
			exit(1);
			break;
	}
}
