#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "instructions.h"
#include "coprocessors/cop0.h"
#include "../devices/devices.h"
#include "../devices/bios.h"
#include "../utils/logger.h"
#include "../utils/ansi.h"

const static char *debugRegisterStrings[32] = {
	"$zero",
	"$at",
	"$v0", "$v1",
	"$a0", "$a1", "$a2", "$a3",
	"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
	"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
	"$t8", "$t9",
	"$k0", "$kl",
	"$gp",
	"$sp",
	"$fp",
	"$ra"
};

void cpu_DumpRegisters(CPU *cpu) {
	int x, y, z;
	int index = 0;
	printf("CPU Registers:\n");
	// Actually the Y axis
	for (x = 0; x < 8; x++) {
		// X axis
		printf("    ");
		for (y = 0; y < 4; y++) {
			printf("%s%*s%s = \x1B[38;5;%dm", ANSI_TEXT_BOLD, 5, debugRegisterStrings[index], ANSI_RESET, 196 + index);
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
	// log_Debug("Current Instruction: 0x%08X, Next Instruction: 0x%08X, PC: 0x%08X, SR: 0x%08X", 
	// 	cpu->this_instruction, cpu->next_instruction, cpu->PC, cpu->SR);
	// Decode instruction bits [31:26]
	switch (cpu->this_instruction >> 26) {
		// SPECIAL
		case SPECIAL:
			instruction_Special(cpu);
			break;
		case LUI:
			instruction_Lui(cpu);
			break;
		case ORI:
			instruction_Ori(cpu);
			break;
		case SW:
			instruction_SW(cpu);
			break;
		case ADDIU:
			instruction_Addiu(cpu);
			break;
		case J:
			instruction_J(cpu);
			break;
		case COP0:
			cop0_Handle(cpu);
			break;
		case BNE:
			instruction_Bne(cpu);
			break;
		case ADDI:
			instruction_Addi(cpu);
			break;
		case LW:
			instruction_Lw(cpu);
			break;
		case SH:
			instruction_Sh(cpu);
			break;
		case JAL:
			instruction_Jal(cpu);
			break;
		case ANDI:
			instruction_Andi(cpu);
			break;
		case SB:
			instruction_Sb(cpu);
			break;
		case LB:
			instruction_Lb(cpu);
			break;
		case BEQ:
			instruction_Beq(cpu);
			break;
		case BGTZ:
			instruction_Bgtz(cpu);
			break;
		case BLEZ:
			instruction_Blez(cpu);
			break;
		case LBU:
			instruction_Lbu(cpu);
			break;
		case SLTIU:
			instruction_Sltiu(cpu);
			break;
		case REGIMM:
			instruction_Bxx(cpu);
			break;
		case SLTI:
			instruction_Slti(cpu);
			break;
		case LHU:
			instruction_Lhu(cpu);
			break;
		case LH:
			instruction_Lh(cpu);
			break;
		case XORI:
			instruction_Xori(cpu);
			break;
		default:
			log_Error("Unhandled Encoded Instruction 0x%08X", cpu->this_instruction);
			cpu_DumpRegisters(cpu);
			exit(1);
			break;
	}
}

void cpu_Exception(CPU *cpu, uint32_t cause) {
	uint32_t handler;
	uint32_t mode;
	// BEV bit exception handler set by cop0
	if ((cpu->SR & (1 << 22)) != 0) {
		handler = 0xbfc00000;
	} else {
		handler = 0x80000080;
	}

	// Interrupt Enable/User Mode & Interrupt Disable/Kernel Mode
	mode = cpu->SR & 0x3f;
	cpu->SR &= ~0x3f;
	cpu->SR |= (mode << 2) & 0x3f;

	cpu->cause = cause << 2;
	cpu->epc = cpu->CURRENT_PC;

	if (cpu->delay_slot) {
		cpu->epc = cpu->epc + 4;
		cpu->cause |= 1 << 31;
	}

	// Direct to handler
	cpu->PC = handler;
	cpu->NEXT_PC = cpu->PC + 4;
}

void cpu_CopyRegister(CPU *cpu) {
	// Copy out register to register
	memcpy(cpu->registers, cpu->out_reg, sizeof(cpu->registers));
}

void cpu_FetchInstruction(CPU *cpu) {
	uint32_t offset = mask_region(cpu->PC);

	switch (offset) {
		// BIOS
		case BIOS_OFFSET ... BIOS_OFFSET + BIOS_SIZE:
			cpu->this_instruction = bios_LoadInt(cpu->devices->bios, offset);
			break;
		// RAM
		case RAM_OFFSET ... RAM_OFFSET + RAM_SIZE:
			cpu->this_instruction = ram_LoadInt(cpu->devices->ram, offset);
			break;
		default:
			log_Error("Unknown PC Region 0x%X", offset);
			exit(1);
			break;
	}
}

void cpu_NextInstruction(CPU *cpu) {
	// Get next instruction
	cpu_FetchInstruction(cpu);

	// Incr to where the next instruction is
	cpu->CURRENT_PC = cpu->PC;  // For exceptions
	// Check alignment
	if ((cpu->CURRENT_PC % 4) != 0) {
		cpu_Exception(cpu, EXCEPITON_LOADADDRERROR);
		return;
	}

	cpu->PC = cpu->NEXT_PC;
	cpu->NEXT_PC = cpu->PC + 4;

	// Handle loading registers
	cpu_SetRegister(cpu, cpu->load[0], cpu->load[1]);
	cpu_SetLoadRegisters(cpu, 0, 0);

	cpu->delay_slot = cpu->branch;
	cpu->branch = false;

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
	cpu->NEXT_PC = cpu->PC + 4;

	// Set status register and HI, LO
	cpu->SR = 0;
	cpu->HI = 0;
	cpu->LO = 0;

	cpu->branch = false;
	cpu->delay_slot = false;

	// Set instructions
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
