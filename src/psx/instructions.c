#include <stdint.h>
#include <stdlib.h>
#include "instructions.h"
#include "cpu.h"
#include "utils.h"
#include "ram.h"
#include "bios.h"
#include "../utils/logger.h"

//
// Helpers
//

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

// [25:21]
uint32_t getS(uint32_t encoded) {
	return (encoded >> 21) & 0x1f;
}

// [25:0]
uint32_t getJump(uint32_t encoded) {
	return encoded & 0x3ffffff;
}

// [20:16]
uint32_t getT(uint32_t encoded) {
	return (encoded >> 16) & 0x1f;
}

// [15:11]
uint32_t getD(uint32_t encoded) {
	return (encoded >> 11) & 0x1f;
}

// [16:0]
uint32_t getI(uint32_t encoded) {
	return encoded & 0xffff;
}

// [16:0] imm sign-extended 32bit
uint32_t getISE(uint32_t encoded) {
	return (uint32_t)((int16_t)getI(encoded));
}

// [10:6]
uint32_t getShift(uint32_t encoded) {
	return (encoded >> 6) & 0x1f;
}

// [5:0]
uint32_t getSubfunc(uint32_t encoded) {
	return encoded & 0x3f;
}

uint8_t load_Memory(uint32_t offset) {
	if ((offset % 4) != 0) {
		log_Error("Unaligned Store 0x%X", offset);
		exit(1);
	}

	log_Warn("MEMORY READ");
	return 0;
}

void store_Memory(CPU *cpu, uint32_t offset, uint32_t value) {
	if ((offset % 4) != 0) {
		log_Error("Unaligned Store 0x%X", offset);
		exit(1);
	}

	// Check addresses
	switch (offset) {
		// If its a bad expansion
		case 0x1f802000:
		case 0x1f000000:
			log_Error("Bad expansion: Address 0x%X", offset);
			exit(1);
			break;

		// Write into MEM_CONTROL
		case MEM_CONTROL ... MEM_CONTROL + MEM_CONTROL_SIZE:
			log_Debug("MEM_CONTROL write");
			break;
		
		// Ignore RAM configuration
		case RAM_CONFIG ... RAM_CONFIG + RAM_CONFIG_SIZE:
			log_Debug("RAM_CONFIG write");
			break;

		// Cache Control
		case CACHE_CONTROL ... CACHE_CONTROL + CACHE_CONTROL_SIZE:
			log_Debug("CACHE_CONTROL write");
			break;

		// RAM Area
		case RAM_OFFSET ... RAM_OFFSET + RAM_SIZE:
			ram_Store32(cpu->devices->ram, offset, value);
			break;

		default:
			log_Error("Not in memory control range, Address 0x%X, Tried to store 0x%X", offset, value);
			exit(1);
			break;
	}
}

void branch(CPU *cpu, uint32_t offset) {
	uint32_t u = offset << 2;
	cpu->PC += u;
	cpu->PC -= 4;
}

// https://www.eg.bucknell.edu/~csci320/mips_web/
// http://problemkaputt.de/psx-spx.htm#cpuregisters
// https://uweb.engr.arizona.edu/~ece369/Resources/spim/MIPSReference.pdf
// General Purpose Registers
// | Register |   Name   |            Use            |
// |----------|----------|---------------------------|
// | $0       | $zero    | Always zero               |
// | $1       | $at      | Assembler temporary       |
// | $2, $3   | $v0, $v1 | Function return values    |
// | $4-$7    | $a0-$a3  | Function arguments        |
// | $8-$15   | $t0-$t7  | Temporary registers       |
// | $16-$23  | $s0-$s7  | Saved registers           |
// | $24-$25  | $t8, $t9 | Temporary registers       |
// | $26-$27  | $k0, $kl | Kernel reserved registers |
// | $28      | $gp      | Global pointer            |
// | $29      | $sp      | Stack pointer             |
// | $30      | $fp      | Frame pointer             |
// | $31      | $ra      | Function return address   |

// Special Purpose Registers
// | Name |                         Description                         |
// |------|-------------------------------------------------------------|
// | PC   | Program Counter                                             |
// | HI   | high 32bits of multiplication result; remainder of division |
// | LO   | low 32bits of multiplication result; quotient of division   |

// Coprocessors (0b0100XX)
// cop0 - exception handling
// cop1 - floating point arithmetic
// cop2 - playstation geometry transformation engine
// cop3 - unimplemented on psx

//
// Instructions
//

void instruction_Lui(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t v = i << 16;  // Low 16 bits set to 0
	
	cpu_SetRegister(cpu, t, v);
	log_Debug("0x%X: lui %s, 0x%X", cpu->this_instruction, debugRegisterStrings[t], i);
}

void instruction_Ori(CPU *cpu) {
	uint32_t i = getI(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, s) | i;

	cpu_SetRegister(cpu, t, v);
	log_Debug("0x%X: ori %s, %s, 0x%X", cpu->this_instruction, debugRegisterStrings[t], debugRegisterStrings[s], i);
}

void instruction_SW(CPU *cpu) {
	// Check if isolated cache
	if ((cpu->SR & 0x10000) != 0) {
		// log_Debug("Ignoring store, isolated cache");
		return;
	}

	uint32_t i = getI(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, t);
	uint32_t addr = cpu_GetRegister(cpu, s) + i;

	store_Memory(cpu, addr, v);

	log_Debug("0x%X: sw %s, 0x%X, %s", cpu->this_instruction, debugRegisterStrings[t], i, debugRegisterStrings[s]);
}

void instruction_Sll(CPU *cpu) {
	uint32_t i = getShift(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t d = getD(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, t) << i;

	cpu_SetRegister(cpu, d, v);

	log_Debug("0x%08X: sll %s, %s, 0x%X", cpu->this_instruction, debugRegisterStrings[d], debugRegisterStrings[t], i);
}

void instruction_Addiu(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, s) + i;

	cpu_SetRegister(cpu, t, v);

	log_Debug("0x%X: addiu %s, %s, 0x%X", cpu->this_instruction, debugRegisterStrings[t], debugRegisterStrings[s], i);
}

void instruction_J(CPU *cpu) {
	uint32_t i = getJump(cpu->this_instruction);
	cpu->PC = (cpu->PC & 0xf0000000) | (i << 2);

	log_Debug("0x%X: j 0x%X", cpu->this_instruction, i);
}

void instruction_Or(CPU *cpu) {
	uint32_t d = getD(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, s) | cpu_GetRegister(cpu, t);

	cpu_SetRegister(cpu, d, v);

	log_Debug("0x%X: or %s, %s, %s", cpu->this_instruction, debugRegisterStrings[d], debugRegisterStrings[s], debugRegisterStrings[t]);
}

void instruction_Mtc0(CPU *cpu) {
	uint32_t cpu_r = getT(cpu->this_instruction);
	uint32_t cop_r = getD(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, cpu_r);

	// Check register
	switch (cop_r) {
		case 12:
			cpu->SR = v;
			break;
		// Cause register
		case 13:
			if (v != 0) {
				log_Error("Unhandled COP0 register");
				exit(1);
			}
			break;
		// Breakpoint registers
		case 3:
		case 5:
		case 6:
		case 7:
		case 9:
		case 11:
			if (v != 0) {
				log_Error("Unhandled write to COP0");
				exit(1);
			}
			break;
		default:
			log_Error("Unknown COP0 Register 0x%X", cop_r);
			exit(1);
			break;
	}

	log_Debug("0x%X: mtc0 %s, %s", cpu->this_instruction, debugRegisterStrings[cpu_r], debugRegisterStrings[cop_r]);
}

void instruction_Bne(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);

	// Branch if Not Equal
	if (cpu_GetRegister(cpu, s) != cpu_GetRegister(cpu, t)) {
		branch(cpu, i);
	}

	log_Debug("0x%X: bne %s, %s, 0x%X", cpu->this_instruction, debugRegisterStrings[s], debugRegisterStrings[t], i);
}

void instruction_Addi(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	int32_t new_s = (int32_t)cpu_GetRegister(cpu, s);
	uint32_t v = new_s + i;

	cpu_SetRegister(cpu, t, v);

	log_Debug("0x%X: addi %s, %s, 0x%X", cpu->this_instruction, debugRegisterStrings[t], debugRegisterStrings[s], i);
}

void instruction_Lw(CPU *cpu) {
	// Isolated Cache Check
	if ((cpu->SR & 0x10000) != 0) {
		return;
	}

	uint32_t i = getISE(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t addr = cpu_GetRegister(cpu, s) + i;
	uint32_t v = load_Memory(addr);

	// Put load in delay slot
	cpu_SetLoadRegisters(cpu, t, v);

	log_Debug("0x%X: lw %s, 0x%X, %s", cpu->this_instruction, debugRegisterStrings[t], i, debugRegisterStrings[s]);
}

void instruction_Beq(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);

	if (cpu_GetRegister(cpu, s) == cpu_GetRegister(cpu, t)) {
		cpu->PC = (cpu->PC + (i << 2)) - 4;
	}

	log_Debug("0x%X: beq %s, %s, 0x%X", cpu->this_instruction, debugRegisterStrings[s], debugRegisterStrings[t], i);
}

void instruction_Sltu(CPU *cpu) {
	uint32_t d = getD(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, s) < cpu_GetRegister(cpu, t);

	cpu_SetRegister(cpu, d, v);

	log_Debug("0x%X: sltu %s, %s, %s", cpu->this_instruction, debugRegisterStrings[d], debugRegisterStrings[s], debugRegisterStrings[t]);
}

void instruction_Addu(CPU *cpu) {
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t d = getD(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, s) + cpu_GetRegister(cpu, t);

	cpu_SetRegister(cpu, d, v);

	log_Debug("0x%X: addu %s, %s, %s", cpu->this_instruction, debugRegisterStrings[d], debugRegisterStrings[s], debugRegisterStrings[t]);
}
