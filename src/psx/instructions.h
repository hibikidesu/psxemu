#ifndef PSX_INSTRUCTIONS
#define PSX_INSTRUCTIONS

#include <stdint.h>
#include "cpu.h"

#define MEM_CONTROL 0x1f801000
#define MEM_CONTROL_SIZE 36
#define RAM_CONFIG  0x1f801060
#define RAM_CONFIG_SIZE 4
#define CACHE_CONTROL 0xfffe0130
#define CACHE_CONTROL_SIZE 4

// Instructions
#define LUI   0b001111
#define ORI   0b001101
#define SW    0b101011
#define SLL   0b000000
#define ADDIU 0b001001
#define J     0b000010
#define OR    0b100101
#define COP0  0b010000
#define MTC0  0b00100
#define BNE   0b000101
#define ADDI  0b001000
#define LW    0b100011
#define BEQ   0b000100
#define SLTU  0b101001
#define ADDU  0b100001

void instruction_Lui(CPU *cpu);
void instruction_Ori(CPU *cpu);
void instruction_SW(CPU *cpu);
void instruction_Sll(CPU *cpu);
void instruction_Addiu(CPU *cpu);
void instruction_J(CPU *cpu);
void instruction_Or(CPU *cpu);
void instruction_Mtc0(CPU *cpu);
void instruction_Bne(CPU *cpu);
void instruction_Addi(CPU *cpu);
void instruction_Lw(CPU *cpu);
void instruction_Beq(CPU *cpu);
void instruction_Sltu(CPU *cpu);

#endif