#include <stdlib.h>
#include "utils/unity/unity.h"
#include "tests.h"
#include "psx/cpu.h"
#include "psx/instructions.h"

static CPU *cpu = NULL;

void setUp() {
	cpu = cpu_Create();
}

void tearDown() {
	free(cpu);
}

void test_function_bios_address(void) {
	TEST_ASSERT_EQUAL(cpu->PC, BIOS_OFFSET);
}

void test_function_ins_lui(void) {
	// lui $t0, 0x13
	cpu->next_instruction = 0x3C080013;
	instruction_Lui(cpu);
	TEST_ASSERT_EQUAL(cpu->out_reg[t0], 0x130000);
}

void test_function_ins_ori(void) {
	// ori $t0, $t0, 0x243F
	cpu->next_instruction = 0x3508243F;
	instruction_Ori(cpu);
	TEST_ASSERT_EQUAL(cpu->out_reg[t0], 0x243f);

	// Copy out register to register
	cpu_CopyRegister(cpu);

	// ori $t1, $t0, 0x1234
	cpu->next_instruction = 0x35091234;
	instruction_Ori(cpu);
	TEST_ASSERT_EQUAL(cpu->out_reg[t1], 0x363f);
}

void test_function_ins_sw(void) {
	// sw $t0, 0x1010, $t1
	// todo
	cpu->next_instruction = 0xAD281010;
	instruction_SW(cpu);
	TEST_ASSERT_EQUAL(cpu->out_reg[t1], 0x130000);
}

void test_function_ins_sll(void) {
	// sll $zero, $zero, 0x0
	cpu->next_instruction = 0x00000000;
	instruction_Sll(cpu);
	TEST_ASSERT_EQUAL(cpu->out_reg[zero], 0x0);

	// sll $t0, $t1, 0x1
	cpu->next_instruction = 0x00094040;
	instruction_Sll(cpu);
	TEST_ASSERT_EQUAL(cpu->out_reg[t1], 0x0);
}

void test_function_ins_addiu(void) {
	// addiu $t0, $t1, 0xB88
	cpu->next_instruction = 0x25280B88;
	instruction_Addiu(cpu);
	TEST_ASSERT_EQUAL(cpu->out_reg[t0], 0xb88);
}

void test_function_ins_j(void) {
	// addiu $t0, $t1, 0xB88
	// todo
	cpu->next_instruction = 0xBF00054;
	instruction_J(cpu);
	TEST_ASSERT_EQUAL(cpu->out_reg[t0], 0xb88);
}

void test_function_ins_or(void) {
	// or $t0, $t1, $t2
	cpu->next_instruction = 0x012A4025;
	cpu->registers[t1] = 0x2;
	cpu->registers[t2] = 0x8;
	instruction_Or(cpu);
	TEST_ASSERT_EQUAL(cpu->out_reg[t0], 0x2 | 0x8);
}

void test_function_ins_bne(void) {
	// bne $t0, $t1, 0xfff7
	// branch equal 0x0 == 0x0, no PC move
	cpu->next_instruction = 0x1509FFF7;
	instruction_Bne(cpu);
	TEST_ASSERT_EQUAL(cpu->PC, BIOS_OFFSET);

	// again except branch not equal, PC move
	cpu->registers[t1] = 0x1;
	instruction_Bne(cpu);
	TEST_ASSERT_EQUAL(cpu->PC, BIOS_OFFSET + (0xfff7 << 2) - 4);
}

int tests_Run() {
	UNITY_BEGIN();
	RUN_TEST(test_function_bios_address);
	RUN_TEST(test_function_ins_lui);
	RUN_TEST(test_function_ins_ori);
	// RUN_TEST(test_function_ins_sw);
	RUN_TEST(test_function_ins_sll);
	RUN_TEST(test_function_ins_addiu);
	// RUN_TEST(test_function_ins_j);
	RUN_TEST(test_function_ins_or);
	RUN_TEST(test_function_ins_bne);
	return UNITY_END();
}
