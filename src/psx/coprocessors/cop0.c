#include <stdlib.h>
#include <stdint.h>
#include "cop0.h"
#include "../cpu.h"
#include "../instructions.h"
#include "../../utils/logger.h"

//
// Processor
//

void cop0_Handle(CPU *cpu) {
	uint32_t instruction = (cpu->next_instruction >> 21) & 0x1f;
	// Find instruction based on COP0 instruction
	switch (instruction) {
		case MTC0:
			instruction_Mtc0(cpu);
			break;
		default:
			log_Error("Unhandled Encoded Instruction COP0: 0x%08X, CPU: 0x%08X", instruction, cpu->next_instruction);
			exit(1);
			break;
	}
}
