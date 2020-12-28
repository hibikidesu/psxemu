#ifndef PSX_CPU
#define PSX_CPU

#include <stdint.h>
#include "../devices/devices.h"

#include <stdint.h>
#include <stdbool.h>

// Exceptions
#define EXCEPTION_INTERRUPT		0x0
#define EXCEPTION_TLB_MOD		0x1
#define EXCEPTION_TLB_LOAD		0x2
#define EXCEPTION_TLB_STORE		0x3
#define EXCEPITON_LOADADDRERROR	0x4
#define EXCEPTION_SAVEADDRERROR	0x5
#define EXCEPTION_BUS_FETCH		0x6
#define EXCEPTION_BUS_STORE		0x7
#define EXCEPTION_SYSCALL		0x8
#define EXCEPTION_BREAK			0x9
#define EXCEPTION_ILLEGAL_INS	0xA
#define EXCEPTION_COPERROR		0xB
#define EXCEPTION_OVERFLOW		0xC
// D-F unused

typedef struct {
	DEVICES *devices;
	int running;
	// General Registers
	uint32_t registers[32];
	// Special Purpose Registers
	uint32_t PC, NEXT_PC;
	uint32_t HI, LO, SR;
	uint32_t this_instruction;
	uint32_t out_reg[32];
	uint32_t load[2];
	// Exceptions
	uint32_t CURRENT_PC;
	uint32_t cause;	// COP0 13 register
	uint32_t epc;	// COP0 14 register
	bool branch;
	bool delay_slot;
} CPU;

void cpu_DumpRegisters(CPU *cpu);
uint32_t cpu_GetRegister(CPU *cpu, uint32_t index);
void cpu_SetRegister(CPU *cpu, uint32_t index, uint32_t value);
void cpu_SetLoadRegisters(CPU *cpu, uint32_t index, uint32_t value);
void cpu_Exception(CPU *cpu, uint32_t cause);
void cpu_ExecuteInstruction(CPU *cpu);
void cpu_CopyRegister(CPU *cpu);
void cpu_FetchInstruction(CPU *cpu);
void cpu_NextInstruction(CPU *cpu);
void cpu_AddDevices(CPU *cpu, DEVICES *devices);
void cpu_Reset(CPU *cpu);
CPU *cpu_Create();
void cpu_Destroy(CPU *cpu);

#endif