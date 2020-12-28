#ifndef PSX_INSTRUCTIONS
#define PSX_INSTRUCTIONS

#include <stdint.h>
#include "cpu.h"

// KUSEG
#define MEM_IO 0x1F801000
#define MEM_IO_SIZE 36

#define RAM_CONFIG  0x1F801060
#define RAM_CONFIG_SIZE 4

// Sound Processing Unit
#define SPU_OFFSET 0x1F801C00
#define SPU_SIZE 640

// Expansion 1
#define EXPANSION_1_OFFSET 0x1f000000
#define EXPANSION_1_SIZE 512 * 1024

// Expansion 2 (I/O Ports)
#define EXPANSION_2_OFFSET 0x1F802000
#define EXPANSION_2_SIZE 66

// Interrupt Registers
#define IRQ_CONTROL_OFFSET 0x1F801070
#define IRQ_CONTROL_SIZE 8

// Timers
#define TIMERS_OFFSET 0x1f801100
#define TIMERS_SIZE 0x30

// GPU
#define GPU_OFFSET 0x1F801810
#define GPU_SIZE 8

// KSEG2
#define CACHE_CONTROL 0xFFFE0130
#define CACHE_CONTROL_SIZE 4

// Registers Defined for debugging
#define zero 0
#define at   1
#define v0   2
#define v1   3
#define a0   4
#define a1   5
#define a2   6
#define a3   7
#define t0   8
#define t1   9
#define t2   10
#define t3   11
#define t4   12
#define t5   13
#define t6   14
#define t7   15
#define s0   16
#define s1   17
#define s2   18
#define s3   19
#define s4   20
#define s5   21
#define s6   22
#define s7   23
#define t8   24
#define t9   25
#define k0   26
#define kl   27
#define gp   28
#define sp   29
#define fp   30
#define ra   31

// Instructions
#define LUI   0b001111
#define ADD   0b100000
#define ADDU  0b100001
#define ADDI  0b001000
#define ADDIU 0b001001
#define AND   0b100100
#define ANDI  0b001100
#define DIV   0b011010
#define DIVU  0b011011
#define MULT  0b011000
#define MULTU 0b011001
#define NOR   0b100111
#define OR    0b100101
#define ORI   0b001101
#define SLL   0b000000
#define SLLV  0b000100
#define SRA   0b000011
#define SRAV  0b000111
#define SRL   0b000010
#define SRLV  0b000110
#define SUB   0b100010
#define SUBU  0b100011
#define XOR   0b100110
#define XORI  0b001110
// #define LHI   0b011001
// #define LLO   0b011000
#define SLT   0b101010
#define SLTU  0b101011
#define SLTI  0b001010
#define SLTIU 0b001011
#define BEQ   0b000100
#define BGTZ  0b000111
#define BLEZ  0b000110
#define BNE   0b000101
#define J     0b000010
#define JAL   0b000011
#define JALR  0b001001
#define JR    0b001000
#define LB    0b100000
#define LBU   0b100100
#define LH    0b100001
#define LHU   0b100101
#define LW    0b100011
#define SB    0b101000
#define SH    0b101001
#define SW    0b101011
#define MFHI  0b010000
#define MFLO  0b010010
#define MTHI  0b010001
#define MTLO  0b010011
#define LWL   0b100010
#define LWR   0b100110
#define SWL   0b101010
#define SWR   0b101110
// #define TRAP  0b011010
#define SYSCALL 0b001100
#define BREAK	0b001101

// Coprocessor Instructions
#define MFC 0b00000
#define CFC 0b00010
#define MTC 0b00100
#define CTC 0b00110
#define RFE 0b10000

#define SPECIAL 0b000000
#define REGIMM  0b000001

// Helpers
uint32_t mask_region(uint32_t address);

// Single instructions
void instruction_Lui(CPU *cpu);
void instruction_Ori(CPU *cpu);
void instruction_SW(CPU *cpu);
void instruction_Sll(CPU *cpu);
void instruction_Addiu(CPU *cpu);
void instruction_J(CPU *cpu);
void instruction_Or(CPU *cpu);
void instruction_Bne(CPU *cpu);
void instruction_Addi(CPU *cpu);
void instruction_Lw(CPU *cpu);
void instruction_Sltu(CPU *cpu);
void instruction_Addu(CPU *cpu);
void instruction_Sh(CPU *cpu);
void instruction_Jal(CPU *cpu);
void instruction_Andi(CPU *cpu);
void instruction_Sb(CPU *cpu);
void instruction_Jr(CPU *cpu);
void instruction_Lb(CPU *cpu);
void instruction_Beq(CPU *cpu);
void instruction_And(CPU *cpu);
void instruction_Add(CPU *cpu);
void instruction_Lbu(CPU *cpu);
void instruction_Bgtz(CPU *cpu);
void instruction_Blez(CPU *cpu);
void instruction_Jalr(CPU *cpu);
void instruction_Slti(CPU *cpu);
void instruction_Sltiu(CPU *cpu);
void instruction_Subu(CPU *cpu);
void instruction_Sra(CPU *cpu);
void instruction_Div(CPU *cpu);
void instruction_Mflo(CPU *cpu);
void instruction_Srl(CPU *cpu);
void instruction_Divu(CPU *cpu);
void instruction_Mfhi(CPU *cpu);
void instruction_Slt(CPU *cpu);
void instruction_Syscall(CPU *cpu);
void instruction_Mtlo(CPU *cpu);
void instruction_Mthi(CPU *cpu);
void instruction_Rfe(CPU *cpu);
void instruction_Lhu(CPU *cpu);
void instruction_Sllv(CPU *cpu);
void instruction_Lh(CPU *cpu);
void instruction_Nor(CPU *cpu);
void instruction_Srav(CPU *cpu);
void instruction_Srlv(CPU *cpu);
void instruction_Multu(CPU *cpu);
void instruction_Xor(CPU *cpu);
void instruction_Break(CPU *cpu);
void instruction_Mult(CPU *cpu);
void instruction_Sub(CPU *cpu);
void instruction_Xori(CPU *cpu);
void instruction_Lwl(CPU *cpu);
void instruction_Lwr(CPU *cpu);
void instruction_Swl(CPU *cpu);
void instruction_Swr(CPU *cpu);

// Special handlers
void instruction_Special(CPU *cpu);
void instruction_Bxx(CPU *cpu);
void instruction_Mtc0(CPU *cpu);
void instruction_Mfc0(CPU *cpu);

#endif