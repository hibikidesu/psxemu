#include <stdlib.h>
#include <stdint.h>
#include "utils/unity/unity.h"
#include "tests.h"
#include "cpu/cpu.h"
#include "devices/devices.h"
#include "devices/ram.h"
#include "cpu/instructions.h"
#include "utils/logger.h"

static CPU *cpu = NULL;

void setUp() {
	DEVICES *devices = devices_Create();
	RAM *ram = ram_Create();
	devices_AddRAM(devices, ram);

	cpu = cpu_Create();
	cpu_AddDevices(cpu, devices);
}

void tearDown() {
	free(cpu->devices->ram);
	free(cpu->devices);
	cpu_Destroy(cpu);
}

void test_function_bios_address(void) {
	TEST_ASSERT_EQUAL_UINT32(cpu->PC, BIOS_OFFSET);
}

//
// Logic
//

void test_function_ins_addu(void) {
	// ADDU $t0, $t0, $t1 (0x0 + 0x0)
	cpu->registers[t0] = 0x0;
	cpu->registers[t1] = 0x0;
	cpu->this_instruction = 0x01094021;
	instruction_Addu(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x0);

	// ADDU $t0, $t0, $t1 (0x0 + 0x1000)
	cpu->registers[t0] = 0x0;
	cpu->registers[t1] = 0x1000;
	cpu->this_instruction = 0x01094021;
	instruction_Addu(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x1000);

	// ADDU $t0, $t0, $t1 (0x1000 + 0x0)
	cpu->registers[t0] = 0x1000;
	cpu->registers[t1] = 0x0;
	cpu->this_instruction = 0x01094021;
	instruction_Addu(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x1000);

	// ADDU $t0, $t0, $t1 (0x1000 + 0x1000)
	cpu->registers[t0] = 0x1000;
	cpu->registers[t1] = 0x1000;
	cpu->this_instruction = 0x01094021;
	instruction_Addu(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x2000);

	// ADDU $t0, $t0, $t1 (0xFFFF + 0x0)
	cpu->registers[t0] = 0xFFFF;
	cpu->registers[t1] = 0x0;
	cpu->this_instruction = 0x01094021;
	instruction_Addu(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0xFFFF);

	// ADDU $t0, $t0, $t1 (0xFFFFFFFF + 0x0)
	cpu->registers[t0] = 0xFFFFFFFF;
	cpu->registers[t1] = 0x0;
	cpu->this_instruction = 0x01094021;
	instruction_Addu(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0xFFFFFFFF);

	// ADDU $t0, $t0, $t1 (0xFFFFFFFF + 0x1)
	// Overflow
	cpu->registers[t0] = 0xFFFFFFFF;
	cpu->registers[t1] = 0x1;
	cpu->this_instruction = 0x01094021;
	instruction_Addu(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0);
}

void test_function_ins_and(void) {
	// AND $t0, $t0, $t1 (0x0 & 0x0)
	cpu->registers[t0] = 0x0;
	cpu->registers[t1] = 0x0;
	cpu->this_instruction = 0x01094024;
	instruction_And(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0);

	// AND $t0, $t0, $t1 (0x0 & 0x20)
	cpu->registers[t0] = 0x0;
	cpu->registers[t1] = 0x20;
	cpu->this_instruction = 0x01094024;
	instruction_And(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x0 & 0x20);

	// AND $t0, $t0, $t1 (0x20 & 0x20)
	cpu->registers[t0] = 0x20;
	cpu->registers[t1] = 0x20;
	cpu->this_instruction = 0x01094024;
	instruction_And(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x20 & 0x20);

	// AND $t0, $t0, $t1 (0xFFFF & 0x8000)
	cpu->registers[t0] = 0xFFFF;
	cpu->registers[t1] = 0x8000;
	cpu->this_instruction = 0x01094024;
	instruction_And(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0xFFFF & 0x8000);

	// AND $t0, $t0, $t1 (0xFFFF & 0xFFFF)
	cpu->registers[t0] = 0xFFFF;
	cpu->registers[t1] = 0xFFFF;
	cpu->this_instruction = 0x01094024;
	instruction_And(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0xFFFF & 0xFFFF);
}

void test_function_ins_or(void) {
	// OR $t0, $t0, $t1 (0x0 & 0x0)
	cpu->registers[t0] = 0x0;
	cpu->registers[t1] = 0x0;
	cpu->this_instruction = 0x01094025;
	instruction_Or(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x0 | 0x0);

	// OR $t0, $t0, $t1 (0x20 & 0x0)
	cpu->registers[t0] = 0x20;
	cpu->registers[t1] = 0x0;
	cpu->this_instruction = 0x01094025;
	instruction_Or(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x20 | 0x0);

	// OR $t0, $t0, $t1 (0x8000 & 0x2000)
	cpu->registers[t0] = 0x8000;
	cpu->registers[t1] = 0x2000;
	cpu->this_instruction = 0x01094025;
	instruction_Or(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x8000 | 0x2000);

	// OR $t0, $t0, $t1 (0xFFFF & 0xFFFF)
	cpu->registers[t0] = 0xFFFF;
	cpu->registers[t1] = 0xFFFF;
	cpu->this_instruction = 0x01094025;
	instruction_Or(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0xFFFF | 0xFFFF);
}

//
// Logic Immediate
//

void test_function_ins_lui(void) {
	// LUI $t0, 0x0
	cpu->this_instruction = 0x3C080000;
	instruction_Lui(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x0 << 16);

	// LUI $t0, 0x32
	cpu->this_instruction = 0x3C080032;
	instruction_Lui(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x32 << 16);

	// LUI $t0, 0xFFFF
	cpu->this_instruction = 0x3C08FFFF;
	instruction_Lui(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0xFFFF << 16);
}

void test_function_ins_addi(void) {
	// ADDI $t0, $t0, 0x0
	cpu->registers[t0] = 0x0;
	cpu->this_instruction = 0x21080000;
	instruction_Addi(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0);

	// ADDI $t0, $t0, 0x0 (0x100 at $t0)
	cpu->registers[t0] = 0x100;
	cpu->this_instruction = 0x21080000;
	instruction_Addi(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x100);
	
	// ADDI $t0, $t0, 0x100
	cpu->registers[t0] = 0x0;
	cpu->this_instruction = 0x21080100;
	instruction_Addi(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x100);

	// ADDI $t0, $t0, 0x100 (0x100 at $t0)
	cpu->registers[t0] = 0x100;
	cpu->this_instruction = 0x21080100;
	instruction_Addi(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x200);

	// ADDI $t0, $t0, 0x1000
	cpu->registers[t0] = 0x0;
	cpu->this_instruction = 0x21081000;
	instruction_Addi(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x1000);
}

void test_function_ins_addiu(void) {
	// ADDIU $t0, $t0, 0x0
	cpu->registers[t0] = 0x0;
	cpu->this_instruction = 0x25080000;
	instruction_Addiu(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0);

	// ADDIU $t0, $t0, 0x0 (0x100 at $t0)
	cpu->registers[t0] = 0x100;
	cpu->this_instruction = 0x25080000;
	instruction_Addiu(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x100);
	
	// ADDIU $t0, $t0, 0x100
	cpu->registers[t0] = 0x0;
	cpu->this_instruction = 0x25080100;
	instruction_Addiu(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x100);

	// ADDIU $t0, $t0, 0x100 (0x100 at $t0)
	cpu->registers[t0] = 0x100;
	cpu->this_instruction = 0x25080100;
	instruction_Addiu(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x200);

	// ADDIU $t0, $t0, 0x1000
	cpu->registers[t0] = 0x0;
	cpu->this_instruction = 0x25081000;
	instruction_Addiu(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x1000);
}

void test_function_ins_andi(void) {
	// ANDI $t0, $t0, 0x0
	cpu->registers[t0] = 0x0;
	cpu->this_instruction = 0x31080000;
	instruction_Andi(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x0 & 0x0);

	// ANDI $t0, $t0, 0x0 (0x100 at $t0)
	cpu->registers[t0] = 0x100;
	cpu->this_instruction = 0x31080000;
	instruction_Andi(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x0 & 0x100);

	// ANDI $t0, $t0, 0x100
	cpu->registers[t0] = 0x0;
	cpu->this_instruction = 0x31080100;
	instruction_Andi(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x0 & 0x100);

	// ANDI $t0, $t0, 0x100 (0x100 at $t0)
	cpu->registers[t0] = 0x100;
	cpu->this_instruction = 0x31080100;
	instruction_Andi(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x100 & 0x100);

	// ANDI $t0, $t0, 0x1000
	cpu->registers[t0] = 0x0;
	cpu->this_instruction = 0x31081000;
	instruction_Andi(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x0 & 0x1000);

	// ANDI $t0, $t0, 0x1000 (0x1000 at $t0)
	cpu->registers[t0] = 0x100;
	cpu->this_instruction = 0x31081000;
	instruction_Andi(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x100 & 0x1000);

	// ANDI $t0, $t0, 0xFFFF (0x1000 at $t0)
	cpu->registers[t0] = 0x1000;
	cpu->this_instruction = 0x3108FFFF;
	instruction_Andi(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x1000 & 0xFFFF);
}

void test_function_ins_ori(void) {
	// ORI $t0, $t0, 0x0
	cpu->registers[t0] = 0x0;
	cpu->this_instruction = 0x35080000;
	instruction_Ori(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0 | 0);

	// ORI $t0, $t0, 0x0 (0x100 at $t0)
	cpu->registers[t0] = 0x100;
	cpu->this_instruction = 0x35080000;
	instruction_Ori(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x100 | 0);

	// ORI $t0, $t0, 0x100
	cpu->registers[t0] = 0x0;
	cpu->this_instruction = 0x35080100;
	instruction_Ori(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x0 | 0x100);

	// ORI $t0, $t0, 0x100 (0x100 at $t0)
	cpu->registers[t0] = 0x100;
	cpu->this_instruction = 0x35080100;
	instruction_Ori(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x100 | 0x100);

	// ORI $t0, $t0, 0xFFFF
	cpu->registers[t0] = 0x0;
	cpu->this_instruction = 0x3508FFFF;
	instruction_Ori(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x0 | 0xFFFF);

	// ORI $t0, $t0, 0xFFFF (0x8000 at $t0)
	cpu->registers[t0] = 0x8000;
	cpu->this_instruction = 0x3508FFFF;
	instruction_Ori(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x8000 | 0xFFFF);
}

void test_function_ins_sll(void) {
	// SLL $zero, $zero, 0x0
	cpu->this_instruction = 0x00000000;
	instruction_Sll(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[zero], 0);

	// SLL $t0, $zero, 0x0
	cpu->this_instruction = 0x00004000;
	instruction_Sll(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0);

	// SLL $t0, $t0, 0x0
	cpu->this_instruction = 0x00084000;
	instruction_Sll(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0);

	// SLL $t0, $t0, 0x0 (0x100 at $t0)
	cpu->registers[t0] = 0x100;
	cpu->this_instruction = 0x00084000;
	instruction_Sll(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x100 << 0);

	// SLL $t0, $t0, 0x10 (0x100 at $t0)
	cpu->registers[t0] = 0x100;
	cpu->this_instruction = 0x00084400;
	instruction_Sll(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x100 << 0x10);

	// SLL $t0, $t0, 0x0 (0xFFFFFFFF at $t0)
	cpu->registers[t0] = 0xFFFFFFFF;
	cpu->this_instruction = 0x00084000;
	instruction_Sll(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0xFFFFFFFF << 0);

	// SLL $t0, $t0, 0x10 (0xFFFFFFFF at $t0)
	cpu->registers[t0] = 0xFFFFFFFF;
	cpu->this_instruction = 0x00084400;
	instruction_Sll(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0xFFFFFFFF << 0x10);
}

void test_function_ins_lb(void) {
	// LB $t0, 0x0($t1)
	cpu->registers[t1] = 0x0;
	cpu->devices->ram->data[0x0] = 0x0;
	cpu->this_instruction = 0x81280000;
	instruction_Lb(cpu);
	TEST_ASSERT_EQUAL_UINT8(cpu->load[0], t0);
	TEST_ASSERT_EQUAL_UINT32(cpu->load[1], 0x0);

	// LB $t0, 0x0($t1) (Mem 0x0 = 0x10)
	cpu->registers[t1] = 0x0;
	cpu->devices->ram->data[0x0] = 0x10;
	cpu->this_instruction = 0x81280000;
	instruction_Lb(cpu);
	TEST_ASSERT_EQUAL_UINT8(cpu->load[0], t0);
	TEST_ASSERT_EQUAL_UINT32(cpu->load[1], 0x10);

	// LB $t0, 0x0($t1) (Mem 0x0 = 0xff)
	cpu->registers[t1] = 0x0;
	cpu->devices->ram->data[0x0] = 0xff;
	cpu->this_instruction = 0x81280000;
	instruction_Lb(cpu);
	TEST_ASSERT_EQUAL_UINT8(cpu->load[0], t0);
	TEST_ASSERT_EQUAL_UINT32(cpu->load[1], (uint32_t)((int8_t)0xff));
}

int tests_Run() {
	UNITY_BEGIN();
	RUN_TEST(test_function_bios_address);
	// Logic
	// MTHIMFHI
	// MTLOMFLO
	// MTHIMFLO
	// MTLOMFHI
	// ADD
	RUN_TEST(test_function_ins_addu);
	// SUB
	// SUBU
	RUN_TEST(test_function_ins_and);
	RUN_TEST(test_function_ins_or);
	// XOR
	// NOR
	// SLLV
	// SRLV
	// SRAV
	// SLT
	// SLTU
	// DIV Q
	// DIV R
	// DIVU Q
	// DIVU R
	// MULT L
	// MULT H
	// MULTU L
	// MULTU H

	// Logic Immediate
	RUN_TEST(test_function_ins_lui);
	RUN_TEST(test_function_ins_addi);
	RUN_TEST(test_function_ins_addiu);
	RUN_TEST(test_function_ins_andi);
	RUN_TEST(test_function_ins_ori);
	// xori
	RUN_TEST(test_function_ins_sll);
	// srl
	// sra
	// slti
	// sltiu

	// Memory
	RUN_TEST(test_function_ins_lb);
	// LBU
	// LH
	// LHU
	// LW
	// LWL
	// LWR
	// SB
	// SH
	// SW
	// SWL
	// SWR
	return UNITY_END();
}
