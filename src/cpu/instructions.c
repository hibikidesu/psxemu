#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "instructions.h"
#include "cpu.h"
#include "../utils/utils.h"
#include "../devices/ram.h"
#include "../devices/bios.h"
#include "../devices/dma.h"
#include "../devices/expansion.h"
#include "../devices/scratchpad.h"
#include "../gpu/gpu.h"
#include "../spu/spu.h"
#include "../devices/cdrom.h"
#include "../devices/joystick.h"
#include "../utils/logger.h"

//
// Helpers
//

// const static char *debugRegisterStrings[32] = {
// 	"$zero",
// 	"$at",
// 	"$v0", "$v1",
// 	"$a0", "$a1", "$a2", "$a3",
// 	"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
// 	"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
// 	"$t8", "$t9",
// 	"$k0", "$kl",
// 	"$gp",
// 	"$sp",
// 	"$fp",
// 	"$ra"
// };

// Region Memory Lookup Table
const static uint32_t REGION_MASK[8] = {
	// KUSEG, 2048MB
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	// KSEG0, 512MB
	0x7fffffff,
	// KSEG1, 512MB
	0x1fffffff,
	// KSEG2, 1024MB
	0xffffffff, 0xffffffff
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

// Get memory region, 512MB chunks
uint32_t mask_region(uint32_t address) {
	return address & REGION_MASK[address >> 29];
}

uint8_t load_Byte(CPU *cpu, uint32_t offset) {
	uint32_t new_offset = mask_region(offset);
	uint8_t value;

	switch (new_offset) {
		// Load from bios
		case BIOS_OFFSET ... BIOS_OFFSET + BIOS_SIZE:
			value = bios_LoadByte(cpu->devices->bios, new_offset);
			break;

		// Expansion 1
		case EXPANSION_1_OFFSET ... EXPANSION_1_OFFSET + EXPANSION_1_SIZE - 1:
			value = expansion1_LoadByte(cpu, new_offset - EXPANSION_1_OFFSET);
			break;

		// RAM
		case RAM_OFFSET ... RAM_OFFSET + RAM_SIZE:
			value = ram_LoadByte(cpu->devices->ram, new_offset);
			break;

		// CDROM
		case CDROM_OFFSET ... CDROM_OFFSET + CDROM_SIZE:
			value = cdrom_LoadByte(cpu->devices->cdrom, new_offset - CDROM_OFFSET);
			break;

		// Scratchpad
		case SCRATCHPAD_OFFSET ... SCRATCHPAD_OFFSET + SCRATCHPAD_SIZE:
			value = scratchpad_LoadByte(cpu->devices->scratchpad, new_offset);
			break;

		// Joystick
		case JOYSTICK_OFFSET ... JOYSTICK_OFFSET + JOYSTICK_SIZE:
			value = joystick_LoadByte(new_offset - JOYSTICK_OFFSET);
			break;

		default:
			log_Error("%s Unkown memory read address 0x%X", 
				__FUNCTION__, new_offset);
			exit(1);
			break;
	}

	return value;
}

uint16_t load_Short(CPU *cpu, uint32_t offset) {
	uint32_t new_offset = mask_region(offset);
	uint16_t value;

	switch (new_offset) {
		// SPU
		case SPU_OFFSET ... SPU_OFFSET + SPU_SIZE:
			log_Debug("Unimplemented SPU short read");
			value = 0x0;
			break;

		// RAM
		case RAM_OFFSET ... RAM_OFFSET + RAM_SIZE:
			value = ram_LoadShort(cpu->devices->ram, new_offset);
			break;

		// IRQ
		case IRQ_CONTROL_OFFSET ... IRQ_CONTROL_OFFSET + IRQ_CONTROL_SIZE:
			log_Debug("Unimplemented IRQ short read");
			value = 0x0;
			break;

		// Scratchpad
		case SCRATCHPAD_OFFSET ... SCRATCHPAD_OFFSET + SCRATCHPAD_SIZE:
			value = scratchpad_LoadShort(cpu->devices->scratchpad, new_offset);
			break;

		// Joystick
		case JOYSTICK_OFFSET ... JOYSTICK_OFFSET + JOYSTICK_SIZE:
			value = joystick_LoadShort(new_offset - JOYSTICK_OFFSET);
			break;

		default:
			log_Error("%s Unkown memory read address 0x%X", 
				__FUNCTION__, new_offset);
			ram_Dump(cpu->devices->ram, "ram.bin");
			exit(1);
			break;
	}

	return value;
}

uint32_t load_Int(CPU *cpu, uint32_t offset) {
	uint32_t value;
	uint32_t new_offset;

	new_offset = mask_region(offset);

	switch (new_offset) {
		// Load from ram
		case RAM_OFFSET ... RAM_OFFSET + RAM_SIZE:
			value = ram_LoadInt(cpu->devices->ram, new_offset);
			break;

		// BIOS
		case BIOS_OFFSET ... BIOS_OFFSET + BIOS_SIZE:
			value = bios_LoadInt(cpu->devices->bios, new_offset);
			break;

		// Interrupt Control
		case IRQ_CONTROL_OFFSET ... IRQ_CONTROL_OFFSET + IRQ_CONTROL_SIZE:
			log_Debug("Unimplemented Interrupt Control Read");
			value = 0x0;
			break;

		// Direct Memory Access
		case DMA_OFFSET ... DMA_OFFSET + DMA_SIZE - 1:
			value = dma_ReadRegister(cpu->devices->dma, new_offset - DMA_OFFSET);
			break;

		// GPU
		case GPU_OFFSET ... GPU_OFFSET + GPU_SIZE:
			switch (new_offset - GPU_OFFSET) {
				// GPUREAD
				case 0:
					value = gpu_Load32(cpu->devices->gpu, new_offset - GPU_OFFSET);
					break;
				// GPUSTAT
				case 4:
					value = gpu_GetStatus(cpu->devices->gpu);
					break;
				default:
					log_Error("%s Unknown GPU offset 0x%X", __FUNCTION__, new_offset - GPU_OFFSET);
					exit(1);
					break;
			}
			break;

		// TIMERS
		case TIMERS_OFFSET ... TIMERS_OFFSET + TIMERS_SIZE:
			log_Debug("Unimplemented timer load");
			value = 0x0;
			break;

		// Scratchpad
		case SCRATCHPAD_OFFSET ... SCRATCHPAD_OFFSET + SCRATCHPAD_SIZE:
			value = scratchpad_LoadInt(cpu->devices->scratchpad, offset);
			break;

		// Expansion 2
		case EXPANSION_2_OFFSET ... EXPANSION_2_OFFSET + EXPANSION_2_SIZE:
			value = expansion2_LoadInt(cpu, new_offset - EXPANSION_2_OFFSET);
			break;

		default:
			log_Error("%s Unkown memory read address 0x%X", 
				__FUNCTION__, new_offset);
			ram_Dump(cpu->devices->ram, "ram.bin");
			exit(1);
			break;
	}

	return value;
}

// Store a value as uint8 (byte)
void store_Byte(CPU *cpu, uint32_t offset, uint8_t value) {
	uint32_t new_offset = mask_region(offset);

	switch (new_offset) {
		// Expansion 2
		case EXPANSION_2_OFFSET ... EXPANSION_2_OFFSET + EXPANSION_2_SIZE:
			expansion2_StoreByte(cpu, new_offset, value);
			break;

		// RAM
		case RAM_OFFSET ... RAM_OFFSET + RAM_SIZE:
			ram_StoreByte(cpu->devices->ram, new_offset, value);
			break;

		// CDROM
		case CDROM_OFFSET ... CDROM_OFFSET + CDROM_SIZE:
			cdrom_LoadByte(cpu->devices->cdrom, new_offset - CDROM_OFFSET);
			break;
		
		// Scratchpad
		case SCRATCHPAD_OFFSET ... SCRATCHPAD_OFFSET + SCRATCHPAD_SIZE:
			scratchpad_StoreByte(cpu->devices->scratchpad, new_offset, value);
			break;

		default:
			log_Error("%s Not in memory control range, Address 0x%X, Tried to store 0x%X", 
				__FUNCTION__, new_offset, value);
			exit(1);
			break;
	}
}

// Store a value as uint16
void store_Short(CPU *cpu, uint32_t offset, uint16_t value) {
	uint32_t new_offset;

	new_offset = mask_region(offset);

	switch (new_offset) {
		// SPU
		case SPU_OFFSET ... SPU_OFFSET + SPU_SIZE:
			log_Debug("%s Unimplemented store to SPU", __FUNCTION__);
			break;

		// TIMERS
		case TIMERS_OFFSET ... TIMERS_OFFSET + TIMERS_SIZE:
			log_Debug("%s Unimplemented timer store", __FUNCTION__);
			break;

		// RAM
		case RAM_OFFSET ... RAM_OFFSET + RAM_SIZE:
			ram_StoreShort(cpu->devices->ram, new_offset, value);
			break;

		// IRQ
		case IRQ_CONTROL_OFFSET ... IRQ_CONTROL_OFFSET + IRQ_CONTROL_SIZE:
			log_Debug("%s Unimplemented IRQ short write", __FUNCTION__);
			break;

		// Scratchpad
		case SCRATCHPAD_OFFSET ... SCRATCHPAD_OFFSET + SCRATCHPAD_SIZE:
			scratchpad_StoreShort(cpu->devices->scratchpad, new_offset, value);
			break;

		// Joystick
		case JOYSTICK_OFFSET ... JOYSTICK_OFFSET + JOYSTICK_SIZE:
			joystick_StoreShort(new_offset - JOYSTICK_OFFSET, value);
			break;

		default:
			log_Error("%s Not in memory control range, Address 0x%X, Tried to store 0x%X", 
				__FUNCTION__, new_offset, value);
			exit(1);
			break;
	}
}

// Store a value as uint32
void store_Int(CPU *cpu, uint32_t offset, uint32_t value) {
	uint32_t new_offset;
	new_offset = mask_region(offset);

	// Check addresses
	switch (new_offset) {
		// Write into MEM_CONTROL
		case MEM_IO ... MEM_IO + MEM_IO_SIZE:
			// log_Debug("MEM_CONTROL write");
			break;
		
		// Ignore RAM configuration
		case RAM_CONFIG ... RAM_CONFIG + RAM_CONFIG_SIZE:
			// log_Debug("RAM_CONFIG write");
			break;

		// Cache Control
		case CACHE_CONTROL ... CACHE_CONTROL + CACHE_CONTROL_SIZE:
			// log_Debug("CACHE_CONTROL write");
			break;

		// IRQ
		case IRQ_CONTROL_OFFSET ... IRQ_CONTROL_OFFSET + IRQ_CONTROL_SIZE:
			log_Debug("%s Unimplemented Interrupt Control Write", __FUNCTION__);
			break;

		// RAM Area
		case RAM_OFFSET ... RAM_OFFSET + RAM_SIZE:
			ram_StoreInt(cpu->devices->ram, new_offset, value);
			break;

		// Direct Memory Access
		case DMA_OFFSET ... DMA_OFFSET + DMA_SIZE - 1:
			devices_DMASetRegister(cpu->devices, new_offset - DMA_OFFSET, value);
			break;

		// GPU
		case GPU_OFFSET ... GPU_OFFSET + GPU_SIZE:
			// Check write
			switch (offset - GPU_OFFSET) {
				case 0:
					gpu_HandleGP0(cpu->devices->gpu, value);
					break;
				case 4:
					gpu_HandleGP1(cpu->devices->gpu, value);
					break;
				default:
					log_Error("%s Unknown GPU Offset 0x%X", __FUNCTION__, offset - GPU_OFFSET);
					exit(1);
					break;
			}
			break;

		// TIMERS
		case TIMERS_OFFSET ... TIMERS_OFFSET + TIMERS_SIZE:
			log_Debug("Unimplemented timer store");
			break;

		// Scratchpad
		case SCRATCHPAD_OFFSET ... SCRATCHPAD_OFFSET + SCRATCHPAD_SIZE:
			scratchpad_StoreInt(cpu->devices->scratchpad, new_offset, value);
			break;

		// Expansion 1
		case EXPANSION_1_OFFSET ... EXPANSION_1_OFFSET + EXPANSION_1_SIZE - 1:
			// Unimplemented
			expansion1_StoreInt(cpu, new_offset, value);
			break;

		default:
			log_Error("%s Not in memory control range, Address 0x%X, Tried to store 0x%X", 
				__FUNCTION__, new_offset, value);
			exit(1);
			break;
	}
}

void branch(CPU *cpu, uint32_t offset) {
	uint32_t u = offset << 2;
	cpu->NEXT_PC = cpu->PC + u;
	cpu->branch = true;
	// cpu->PC -= 4;
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
	uint32_t i = getI(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t v = i << 16;

	cpu_SetRegister(cpu, t, v);
	// log_Debug("0x%X: lui %s, 0x%04X", cpu->this_instruction, debugRegisterStrings[t], i);
}

void instruction_Ori(CPU *cpu) {
	uint32_t i = getI(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, s) | i;

	cpu_SetRegister(cpu, t, v);
	// log_Debug("0x%X: ori %s, %s, 0x%04X", cpu->this_instruction, debugRegisterStrings[t], debugRegisterStrings[s], i);
}

void instruction_SW(CPU *cpu) {
	// Check if isolated cache
	if ((cpu->SR & 0x10000) != 0) {
		// log_Debug("Ignoring store, isolated cache");
		return;
	}

	uint32_t i = getISE(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);

	uint32_t addr = cpu_GetRegister(cpu, s) + i;
	uint32_t v = cpu_GetRegister(cpu, t);

	if ((addr % 4) == 0) {
		store_Int(cpu, addr, v);
	} else {
		cpu_Exception(cpu, EXCEPTION_SAVEADDRERROR);
	}

	// log_Debug("0x%X: sw %s, 0x%X, %s", cpu->this_instruction, debugRegisterStrings[t], i, debugRegisterStrings[s]);
}

void instruction_Sll(CPU *cpu) {
	uint32_t i = getShift(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t d = getD(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, t) << i;

	cpu_SetRegister(cpu, d, v);

	// log_Debug("0x%08X: sll %s, %s, 0x%X", cpu->this_instruction, debugRegisterStrings[d], debugRegisterStrings[t], i);
}

void instruction_Addiu(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, s) + i;

	cpu_SetRegister(cpu, t, v);

	// log_Debug("0x%X: addiu %s, %s, 0x%X", cpu->this_instruction, debugRegisterStrings[t], debugRegisterStrings[s], i);
}

void instruction_J(CPU *cpu) {
	uint32_t i = getJump(cpu->this_instruction);
	cpu->NEXT_PC = (cpu->PC & 0xf0000000) | (i << 2);
	cpu->branch = true;
	// log_Debug("JUMP TO 0x%X", cpu->PC);

	// log_Debug("0x%X: j 0x%X", cpu->this_instruction, i);
}

void instruction_Or(CPU *cpu) {
	uint32_t d = getD(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, s) | cpu_GetRegister(cpu, t);

	cpu_SetRegister(cpu, d, v);

	// log_Debug("0x%X: or %s, %s, %s", cpu->this_instruction, debugRegisterStrings[d], debugRegisterStrings[s], debugRegisterStrings[t]);
}

void instruction_Bne(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);

	// Branch if Not Equal
	if (cpu_GetRegister(cpu, s) != cpu_GetRegister(cpu, t)) {
		branch(cpu, i);
	}

	// log_Debug("0x%X: bne %s, %s, 0x%X", cpu->this_instruction, debugRegisterStrings[s], debugRegisterStrings[t], i);
}

void instruction_Addi(CPU *cpu) {
	int32_t i = getISE(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	int32_t new_s = (int32_t)cpu_GetRegister(cpu, s);

	// Check for overflow
	int32_t v;
	if (__builtin_sadd_overflow(new_s, i, &v)) {
		cpu_Exception(cpu, EXCEPTION_OVERFLOW);
		return;
	}

	cpu_SetRegister(cpu, t, (uint32_t)v);

	// log_Debug("0x%X: addi %s, %s, 0x%X", cpu->this_instruction, debugRegisterStrings[t], debugRegisterStrings[s], i);
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
	uint32_t v = load_Int(cpu, addr);

	if ((addr % 4) == 0) {
		// Put load in delay slot
		cpu_SetLoadRegisters(cpu, t, v);
	} else {
		cpu_Exception(cpu, EXCEPITON_LOADADDRERROR);
	}

	// log_Debug("0x%X: lw %s, 0x%X, %s", cpu->this_instruction, debugRegisterStrings[t], i, debugRegisterStrings[s]);
}

void instruction_Sltu(CPU *cpu) {
	uint32_t d = getD(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, s) < cpu_GetRegister(cpu, t);

	cpu_SetRegister(cpu, d, v);

	// log_Debug("0x%X: sltu %s, %s, %s", cpu->this_instruction, debugRegisterStrings[d], debugRegisterStrings[s], debugRegisterStrings[t]);
}

void instruction_Addu(CPU *cpu) {
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t d = getD(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, s) + cpu_GetRegister(cpu, t);

	cpu_SetRegister(cpu, d, v);

	// log_Debug("0x%X: addu %s, %s, %s", cpu->this_instruction, debugRegisterStrings[d], debugRegisterStrings[s], debugRegisterStrings[t]);
}

void instruction_Sh(CPU *cpu) {
	if ((cpu->SR & 0x10000) != 0) {
		// log_Debug("Ignoring store, isolated cache");
		return;
	}
	
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t addr = cpu_GetRegister(cpu, s) + i;

	if ((addr % 2) == 0) {
		store_Short(cpu, addr, (uint16_t)cpu_GetRegister(cpu, t));
	} else {
		cpu_Exception(cpu, EXCEPTION_SAVEADDRERROR);
	}
	
	// log_Debug("0x%X: sh %s, 0x%X, %s", cpu->this_instruction, debugRegisterStrings[t], i, debugRegisterStrings[s]);
}
 
void instruction_Jal(CPU *cpu) {
	// Remember our program counter in the ra register
	cpu_SetRegister(cpu, ra, cpu->NEXT_PC);

	// Jump
	instruction_J(cpu);

	cpu->branch = true;

	// log_Debug("0x%X: jal", cpu->this_instruction);
}

void instruction_Andi(CPU *cpu) {
	uint32_t i = getI(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, s) & i;

	cpu_SetRegister(cpu, t, v);

	// log_Debug("0x%X: andi %s, %s, 0x%X", cpu->this_instruction, debugRegisterStrings[t], debugRegisterStrings[s], i);
}

void instruction_Sb(CPU *cpu) {
	if ((cpu->SR & 0x10000) != 0) {
		return;
	}

	uint32_t i = getISE(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t addr = cpu_GetRegister(cpu, s) + i;
	
	store_Byte(cpu, addr, (uint8_t)cpu_GetRegister(cpu, t));

	// log_Debug("0x%X: sb %s, 0x%X, %s", cpu->this_instruction, debugRegisterStrings[t], i, debugRegisterStrings[s]);
}

void instruction_Jr(CPU *cpu) {
	uint32_t s = getS(cpu->this_instruction);
	cpu->NEXT_PC = cpu_GetRegister(cpu, s);
	cpu->branch = true;
	// log_Debug("0x%X: jr %s", cpu->this_instruction, debugRegisterStrings[s]);
}

void instruction_Lb(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t addr = cpu_GetRegister(cpu, s) + i;
	int8_t v = (int8_t)load_Byte(cpu, addr);

	cpu_SetLoadRegisters(cpu, t, (uint32_t)v);

	// log_Debug("0x%X: lb %s, 0x%X, %s", cpu->this_instruction, debugRegisterStrings[t], i, debugRegisterStrings[s]);
}

void instruction_Beq(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);

	if (cpu_GetRegister(cpu, s) == cpu_GetRegister(cpu, t)) {
		branch(cpu, i);
	}

	// log_Debug("0x%X: beq %s, %s, 0x%X", cpu->this_instruction, debugRegisterStrings[s], debugRegisterStrings[t], i);
}

void instruction_And(CPU *cpu) {
	uint32_t d = getD(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, s) & cpu_GetRegister(cpu, t);

	cpu_SetRegister(cpu, d, v);

	// log_Debug("0x%X: and %s, %s, %s", cpu->this_instruction, debugRegisterStrings[d], debugRegisterStrings[s], debugRegisterStrings[t]);
}

void instruction_Add(CPU *cpu) {
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t d = getD(cpu->this_instruction);

	int32_t s_new = (int32_t)cpu_GetRegister(cpu, s);
	int32_t t_new = (int32_t)cpu_GetRegister(cpu, t);

	// Check for overflow
	int32_t v;
	if (__builtin_sadd_overflow(s_new, t_new, &v)) {
		cpu_Exception(cpu, EXCEPTION_OVERFLOW);
		return;
	}

	cpu_SetRegister(cpu, d, (uint32_t)v);
}

void instruction_Lbu(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t addr = cpu_GetRegister(cpu, s) + i;
	uint32_t v = load_Byte(cpu, addr);

	cpu_SetLoadRegisters(cpu, t, v);
}

void instruction_Bgtz(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	int32_t v = (int32_t)cpu_GetRegister(cpu, s);

	if (v > 0) {
		branch(cpu, i);
	}
}

void instruction_Blez(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	int32_t v = (int32_t)cpu_GetRegister(cpu, s);

	if (v <= 0) {
		branch(cpu, i);
	}
}

void instruction_Jalr(CPU *cpu) {
	uint32_t d = getD(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	cpu_SetRegister(cpu, d, cpu->NEXT_PC);
	cpu->NEXT_PC = cpu_GetRegister(cpu, s);
	cpu->branch = true;
}

void instruction_Slti(CPU *cpu) {
	int32_t i = getISE(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t v = ((int32_t)cpu_GetRegister(cpu, s)) < i;

	cpu_SetRegister(cpu, t, (uint32_t)v);
}

void instruction_Sltiu(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, s) < i;

	cpu_SetRegister(cpu, t, v);
}

void instruction_Subu(CPU *cpu) {
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t d = getD(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, s) - cpu_GetRegister(cpu, t);

	cpu_SetRegister(cpu, d, v);
}

void instruction_Sra(CPU *cpu) {
	uint32_t i = getShift(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t d = getD(cpu->this_instruction);
	int32_t v = ((int32_t)cpu_GetRegister(cpu, t)) >> i;

	cpu_SetRegister(cpu, d, (uint32_t)v);
}

void instruction_Div(CPU *cpu) {
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	int32_t n = (int32_t)cpu_GetRegister(cpu, s);
	int32_t d = (int32_t)cpu_GetRegister(cpu, t);

	if (d == 0) {
		// Division By 0
		cpu->HI = (uint32_t)n;
		if (n >= 0) {
			cpu->LO = 0xffffffff;
		} else {
			cpu->LO = 1;
		}
	} else if (((uint32_t)n) == 0x80000000 && d == -1) {
		// If not representable in 32bit
		// Signed int
		cpu->HI = 0;
		cpu->LO = 0x80000000;
	} else {
		cpu->HI = (uint32_t)(n % d);
		cpu->LO = (uint32_t)(n / d);
	}
}

// Move LO to a register
void instruction_Mflo(CPU *cpu) {
	uint32_t d = getD(cpu->this_instruction);
	cpu_SetRegister(cpu, d, cpu->LO);
}

void instruction_Srl(CPU *cpu) {
	uint32_t i = getShift(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t d = getD(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, t) >> i;

	cpu_SetRegister(cpu, d, v);
}

// Division Unsigned
void instruction_Divu(CPU *cpu) {
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);

	uint32_t n = cpu_GetRegister(cpu, s);
	uint32_t d = cpu_GetRegister(cpu, t);

	if (d == 0) {
		// Division By 0
		cpu->HI = n;
		cpu->LO = 0xffffffff;
	} else {
		cpu->HI = n % d;
		cpu->LO = n / d;
	}
}

// Move From HI
void instruction_Mfhi(CPU *cpu) {
	uint32_t d = getD(cpu->this_instruction);
	cpu_SetRegister(cpu, d, cpu->HI);
}

// Set Less Than
void instruction_Slt(CPU *cpu) {
	uint32_t d = getD(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);

	int32_t ns = (int32_t)cpu_GetRegister(cpu, s);
	int32_t nt = (int32_t)cpu_GetRegister(cpu, t);
	
	cpu_SetRegister(cpu, d, (uint32_t)(ns < nt));
}

void instruction_Syscall(CPU *cpu) {
	cpu_Exception(cpu, EXCEPTION_SYSCALL);
}

// Move To LO
void instruction_Mtlo(CPU *cpu) {
	uint32_t s = getS(cpu->this_instruction);
	cpu->LO = cpu_GetRegister(cpu, s);
}

// Move To HI
void instruction_Mthi(CPU *cpu) {
	uint32_t s = getS(cpu->this_instruction);
	cpu->HI = cpu_GetRegister(cpu, s);
}

void instruction_Rfe(CPU *cpu) {
	if ((cpu->this_instruction & 0x3f) != 0b010000) {
		log_Error("Invalid instruction 0x%X", cpu->this_instruction);
		exit(1);
	}

	uint32_t mode = cpu->SR & 0x3f;
	cpu->SR &= ~0x3f;
	cpu->SR |= mode >> 2;
}

// Load Halfword Unsigned
void instruction_Lhu(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t addr = cpu_GetRegister(cpu, s) + i;

	if ((addr % 2) == 0) {
		cpu_SetLoadRegisters(cpu, t, (uint32_t)load_Short(cpu, addr));
	} else {
		cpu_Exception(cpu, EXCEPITON_LOADADDRERROR);
	}
}

// Shift Left Logical Value
void instruction_Sllv(CPU *cpu) {
	uint32_t d = getD(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, t) << (cpu_GetRegister(cpu, s) & 0x1f);

	cpu_SetRegister(cpu, d, v);
}

// Load Halfword
void instruction_Lh(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t addr = cpu_GetRegister(cpu, s) + i;

	if ((addr % 2) == 0) {
		int16_t v = (int16_t)load_Short(cpu, addr);
		cpu_SetLoadRegisters(cpu, t, (uint32_t)v);
	} else {
		cpu_Exception(cpu, EXCEPITON_LOADADDRERROR);
	}
}

void instruction_Nor(CPU *cpu) {
	uint32_t d = getD(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t v = ~(cpu_GetRegister(cpu, s) | cpu_GetRegister(cpu, t));

	cpu_SetRegister(cpu, d, v);
}

// Shift Word Right Arithmetic Variable
void instruction_Srav(CPU *cpu) {
	uint32_t d = getD(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t v = ((int32_t)cpu_GetRegister(cpu, t)) >> (cpu_GetRegister(cpu, s) & 0x1f);

	cpu_SetRegister(cpu, d, v);
}

// Shift Word Right Logical Variable
void instruction_Srlv(CPU *cpu) {
	uint32_t d = getD(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, t) >> (cpu_GetRegister(cpu, s) & 0x1f);

	cpu_SetRegister(cpu, d, v);
}

// Multily Unsigned Word
void instruction_Multu(CPU *cpu) {
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);

	uint64_t a = cpu_GetRegister(cpu, s);
	uint64_t b = cpu_GetRegister(cpu, t);
	uint64_t v = a * b;

	cpu->HI = (uint32_t)(v >> 32);
	cpu->LO = (uint32_t)v;
}

// Exclusive OR
void instruction_Xor(CPU *cpu) {
	uint32_t d = getD(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, s) ^ cpu_GetRegister(cpu, t);

	cpu_SetRegister(cpu, d, v);
}

// Breakpoint
void instruction_Break(CPU *cpu) {
	cpu_Exception(cpu, EXCEPTION_BREAK);
}

// Multiply Word
void instruction_Mult(CPU *cpu) {
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);

	int64_t a = (int64_t)((int32_t)cpu_GetRegister(cpu, s));
	int64_t b = (int64_t)((int32_t)cpu_GetRegister(cpu, t));
	uint64_t v = a * b;

	cpu->HI = (uint32_t)(v >> 32);
	cpu->LO = (uint32_t)v;
}

// Subtract Word
void instruction_Sub(CPU *cpu) {
	uint32_t s = getS(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t d = getD(cpu->this_instruction);

	int32_t ns = (int32_t)cpu_GetRegister(cpu, s);
	int32_t nt = (int32_t)cpu_GetRegister(cpu, t);
	int32_t v;

	// Check unsigned overflow
	if(__builtin_ssub_overflow(ns, nt, &v)) {
		cpu_Exception(cpu, EXCEPTION_OVERFLOW);
		return;
	}

	cpu_SetRegister(cpu, d, (uint32_t)v);
}

// Exclusive OR Immediate
void instruction_Xori(CPU *cpu) {
	uint32_t i = getI(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t v = cpu_GetRegister(cpu, s) ^ i;

	cpu_SetRegister(cpu, t, v);
}

// Load Word Left
void instruction_Lwl(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t addr = cpu_GetRegister(cpu, s) + i;

	// Merge with value currently being loaded.
	uint32_t cur_v = cpu->out_reg[t];

	// Load aligned word with aligned address
	uint32_t aligned_word = load_Int(cpu, addr & ~3);
	uint32_t v;

	// Get most sig bits depending on alignment
	switch (addr & 3) {
		case 0:
			v = (cur_v & 0x00ffffff) | (aligned_word << 24);
			break;
		case 1:
			v = (cur_v & 0x0000ffff) | (aligned_word << 16);
			break;
		case 2:
			v = (cur_v & 0x000000ff) | (aligned_word << 8);
			break;
		case 3:
			v = (cur_v & 0x00000000) | (aligned_word << 0);
			break;
		default:
			log_Error("%s Unkown Value", __FUNCTION__);
			exit(1);
			break;
	}

	// Set into load delay register
	cpu_SetLoadRegisters(cpu, t, v);
}

// Load Word Right
void instruction_Lwr(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t addr = cpu_GetRegister(cpu, s) + i;

	// Merge with value currently being loaded.
	uint32_t cur_v = cpu->out_reg[t];

	// Load aligned word with aligned address
	uint32_t aligned_word = load_Int(cpu, addr & ~3);
	uint32_t v;

	// Get least sig bits depending on alignment
	switch (addr & 3) {
		case 0:
			v = (cur_v & 0x00000000) | (aligned_word >> 0);
			break;
		case 1:
			v = (cur_v & 0xff000000) | (aligned_word >> 8);
			break;
		case 2:
			v = (cur_v & 0xffff0000) | (aligned_word >> 16);
			break;
		case 3:
			v = (cur_v & 0xffffff00) | (aligned_word >> 24);
			break;
		default:
			log_Error("%s Unkown Value", __FUNCTION__);
			exit(1);
			break;
	}

	// Set into load delay register
	cpu_SetLoadRegisters(cpu, t, v);
}

// Store Word Left
void instruction_Swl(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t addr = cpu_GetRegister(cpu, s) + i;
	uint32_t v = cpu_GetRegister(cpu, t);

	// Load memory from aligned address
	uint32_t cur_mem = load_Int(cpu, addr & ~3);
	uint32_t mem;

	// From most sig bits
	switch (addr & 3) {
		case 0:
			mem = (cur_mem & 0xffffff00) | (v >> 24);
			break;
		case 1:
			mem = (cur_mem & 0xffff0000) | (v >> 16);
			break;
		case 2:
			mem = (cur_mem & 0xff000000) | (v >> 8);
			break;
		case 3:
			mem = (cur_mem & 0x00000000) | (v >> 0);
			break;
		default:
			log_Error("%s Unkown Value", __FUNCTION__);
			exit(1);
			break;
	}

	// Write memory to aligned addr
	store_Int(cpu, addr & ~3, mem);
}

// Store Word Right
void instruction_Swr(CPU *cpu) {
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t t = getT(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);
	uint32_t addr = cpu_GetRegister(cpu, s) + i;
	uint32_t v = cpu_GetRegister(cpu, t);

	// Load memory from aligned address
	uint32_t cur_mem = load_Int(cpu, addr & ~3);
	uint32_t mem;

	// From least sig bits
	switch (addr & 3) {
		case 0:
			mem = (cur_mem & 0x00000000) | (v << 0);
			break;
		case 1:
			mem = (cur_mem & 0x000000ff) | (v << 8);
			break;
		case 2:
			mem = (cur_mem & 0x0000ffff) | (v << 16);
			break;
		case 3:
			mem = (cur_mem & 0x00ffffff) | (v << 24);
			break;
		default:
			log_Error("%s Unkown Value", __FUNCTION__);
			exit(1);
			break;
	}

	// Write memory to aligned addr
	store_Int(cpu, addr & ~3, mem);
}

//
// SPECIAL HANDLERS
//

void instruction_Special(CPU *cpu) {
	switch (getSubfunc(cpu->this_instruction)) {
		case SLL:
			instruction_Sll(cpu);
			break;
		case OR:
			instruction_Or(cpu);
			break;
		case SLTU:
			instruction_Sltu(cpu);
			break;
		case ADDU:
			instruction_Addu(cpu);
			break;
		case JR:
			instruction_Jr(cpu);
			break;
		case AND:
			instruction_And(cpu);
			break;
		case ADD:
			instruction_Add(cpu);
			break;
		case JALR:
			instruction_Jalr(cpu);
			break;
		case SUBU:
			instruction_Subu(cpu);
			break;
		case SRA:
			instruction_Sra(cpu);
			break;
		case DIV:
			instruction_Div(cpu);
			break;
		case MFLO:
			instruction_Mflo(cpu);
			break;
		case SRL:
			instruction_Srl(cpu);
			break;
		case DIVU:
			instruction_Divu(cpu);
			break;
		case MFHI:
			instruction_Mfhi(cpu);
			break;
		case SLT:
			instruction_Slt(cpu);
			break;
		case SYSCALL:
			instruction_Syscall(cpu);
			break;
		case MTLO:
			instruction_Mtlo(cpu);
			break;
		case MTHI:
			instruction_Mthi(cpu);
			break;
		case SLLV:
			instruction_Sllv(cpu);
			break;
		case NOR:
			instruction_Nor(cpu);
			break;
		case SRAV:
			instruction_Srav(cpu);
			break;
		case SRLV:
			instruction_Srlv(cpu);
			break;
		case MULTU:
			instruction_Multu(cpu);
			break;
		case XOR:
			instruction_Xor(cpu);
			break;
		case BREAK:
			instruction_Break(cpu);
			break;
		case MULT:
			instruction_Mult(cpu);
			break;
		case SUB:
			instruction_Sub(cpu);
			break;
		default:
			log_Error("Unhandled SPECIAL Encoded Instruction 0x%08X, Subfunc 0x%X", 
				cpu->this_instruction, getSubfunc(cpu->this_instruction));
			exit(1);
			break;
	}
}

void instruction_Bxx(CPU *cpu) {
	// notlikebep
	uint32_t i = getISE(cpu->this_instruction);
	uint32_t s = getS(cpu->this_instruction);

	bool isBgez = (cpu->this_instruction >> 16) & 1;
	bool isLink = ((cpu->this_instruction >> 17) & 0xf) == 8;

	int32_t v = (int32_t)cpu_GetRegister(cpu, s);
	uint32_t test = ((uint32_t)(v < 0)) ^ isBgez;

	if (isLink) {
		cpu_SetRegister(cpu, ra, cpu->NEXT_PC);
	}

	if (test != 0) {
		branch(cpu, i);
	}
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
			cpu->cause = v;
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

	// log_Debug("0x%X: mtc0 %s, %s", cpu->this_instruction, debugRegisterStrings[cpu_r], debugRegisterStrings[cop_r]);
}

void instruction_Mfc0(CPU *cpu) {
	uint32_t cpu_r = getT(cpu->this_instruction);
	uint32_t cop_r = getD(cpu->this_instruction);
	uint32_t v = 0;

	switch (cop_r) {
		// JUMPDEST
		case 6:
			log_Warn("Unimplemented JUMPDEST");
			break;
		// DCIC
		case 7:
			log_Warn("Unimplemented DCIC");
			break;
		// Bad Virtual Address
		case 8:
			log_Warn("Unimplemented bad virtual address");
			break;
		// SR
		case 12:
			v = cpu->SR;
			break;
		// Cause register
		case 13:
			v = cpu->cause;
			break;
		// EPC
		case 14:
			v = cpu->epc;
			break;
		// Processor ID
		case 15:
			v = 0x2;
			break;
		default:
			log_Error("Unknown MFC0 Register 0x%X", cop_r);
			exit(1);
			break;
	}

	// Set delay
	cpu_SetLoadRegisters(cpu, cpu_r, v);
}
