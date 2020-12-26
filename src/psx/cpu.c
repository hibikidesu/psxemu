#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "bios.h"
#include "instructions.h"
#include "coprocessors/cop0.h"
#include "../utils/logger.h"
#include "../utils/ansi.h"

void cpu_DumpRegisters(CPU *cpu) {
	int x, y, z;
	int index = 0;
	printf("CPU Registers:\n");
	// Actually the Y axis
	for (x = 0; x < 8; x++) {
		// X axis
		printf("    ");
		for (y = 0; y < 4; y++) {
			printf("%sR%02d%s = \x1B[38;5;%dm", ANSI_TEXT_BOLD, index, ANSI_RESET, 196 + index);
			// Hex print
			for (z = 0; z < 4; z++) {
				printf("%02X ", (cpu->registers[index] >> (8 * z)) & 0xff);
			}
			printf("%s", ANSI_RESET);
			index++;
		}
		printf("\n");
	}
}

uint32_t cpu_GetRegister(CPU *cpu, uint32_t index) {
	return cpu->registers[index];
}

void cpu_SetRegister(CPU *cpu, uint32_t index, uint32_t value) {
	cpu->out_reg[index] = value;
	// REGISTER $ZERO WILL ALWAYS STAY 0
	cpu->out_reg[0] = 0;
}

void cpu_SetLoadRegisters(CPU *cpu, uint32_t index, uint32_t value) {
	cpu->load[0] = index;
	cpu->load[1] = value;
}

void cpu_ExecuteInstruction(CPU *cpu) {
	// Decode instruction bits [31:26]
	switch (cpu->this_instruction >> 26) {
		case LUI:
			instruction_Lui(cpu);
			break;
		case ORI:
			instruction_Ori(cpu);
			break;
		case SW:
			instruction_SW(cpu);
			break;
		case SLL:
			instruction_Sll(cpu);
			break;
		case ADDI:
			instruction_Addi(cpu);
			break;
		case ADDIU:
			instruction_Addiu(cpu);
			break;
		case J:
			instruction_J(cpu);
			break;
		case OR:
			instruction_Or(cpu);
			break;
		case COP0:
			cop0_Handle(cpu);
			break;
		case BNE:
			instruction_Bne(cpu);
			break;
		case LW:
			instruction_Lw(cpu);
			break;
		// case BEQ:
		// 	instruction_Beq(cpu);
		// 	break;
		// case SLTU:
		// 	instruction_Sltu(cpu);
		// 	break;
		default:
			log_Error("Unhandled Encoded Instruction 0x%08X", cpu->this_instruction);
			cpu_DumpRegisters(cpu);
			exit(1);
			break;
	}
}

void cpu_CopyRegister(CPU *cpu) {
	// Copy out register to register
	memcpy(cpu->registers, cpu->out_reg, sizeof(cpu->registers));
}

void cpu_NextInstruction(CPU *cpu) {
	// Handle loading registers
	cpu_SetRegister(cpu, cpu->load[0], cpu->load[1]);
	cpu_SetLoadRegisters(cpu, 0, 0);

	// Set this instruction
	cpu->this_instruction = cpu->next_instruction;

	// Get next instruction
	cpu->next_instruction = bios_Load32(cpu->devices->bios, cpu->PC);

	// Incr to where the next instruction is
	cpu->PC += 4;

	// Execute instruction
	cpu_ExecuteInstruction(cpu);

	cpu_CopyRegister(cpu);
}

void cpu_AddDevices(CPU *cpu, DEVICES *devices) {
	cpu->devices = devices;
}

void cpu_Reset(CPU *cpu) {
	// Beginning of bios
	cpu->PC = BIOS_OFFSET;

	// Set status register
	cpu->SR = 0;

	// Set instructions
	cpu->next_instruction = 0;
	cpu->this_instruction = 0;

	// Reset out and load registers
	cpu_SetLoadRegisters(cpu, 0, 0);
	memset(cpu->out_reg, 0, sizeof(cpu->out_reg));

	// Clear registers
	memset(cpu->registers, 0, sizeof(cpu->registers));
}

CPU *cpu_Create() {
	// Allocate CPU struct
	CPU *cpu = malloc(sizeof(CPU));

	// Reset CPU counter to bios
	cpu_Reset(cpu);
	cpu->running = 0;

	// Return CPU object
	return cpu;
}

void cpu_Destroy(CPU *cpu) {
	free(cpu);
}
