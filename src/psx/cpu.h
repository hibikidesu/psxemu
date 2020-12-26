#ifndef PSX_CPU
#define PSX_CPU

#include <stdint.h>
#include "devices.h"

#include <stdint.h>

typedef struct {
	DEVICES *devices;
	int running;
	// General Registers
	uint32_t registers[32];
	// Special Purpose Registers
	uint32_t PC, HI, LO, SR;
	
	uint32_t next_instruction;
	uint32_t this_instruction;
	uint32_t out_reg[32];
	uint32_t load[2];
} CPU;

void cpu_DumpRegisters(CPU *cpu);
uint32_t cpu_GetRegister(CPU *cpu, uint32_t index);
void cpu_SetRegister(CPU *cpu, uint32_t index, uint32_t value);
void cpu_SetLoadRegisters(CPU *cpu, uint32_t index, uint32_t value);
void cpu_ExecuteInstruction(CPU *cpu);
void cpu_CopyRegister(CPU *cpu);
void cpu_NextInstruction(CPU *cpu);
void cpu_AddDevices(CPU *cpu, DEVICES *devices);
void cpu_Reset(CPU *cpu);
CPU *cpu_Create();
void cpu_Destroy(CPU *cpu);

#endif