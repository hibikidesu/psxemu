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
	// log_Debug("COPROCESSOR 0 CALLED");
	uint32_t instruction = (cpu->this_instruction >> 21) & 0x1f;
	// Find instruction based on COP0 instruction
	switch (instruction) {
		case MTC:
			instruction_Mtc0(cpu);
			break;
		case MFC:
			instruction_Mfc0(cpu);
			break;
		default:
			log_Error("Unhandled Encoded Instruction COP0: 0x%08X, CPU: 0x%08X", instruction, cpu->this_instruction);
			exit(1);
			break;
	}
}
