#include <stdlib.h>
#include <stdint.h>
#include "utils/unity/unity.h"
#include "tests.h"
#include "cpu/cpu.h"
#include "cpu/instructions.h"
#include "utils/logger.h"

static CPU *cpu = NULL;

void setUp() {
	cpu = cpu_Create();
}

void tearDown() {
	free(cpu);
}

void test_function_bios_address(void) {
	TEST_ASSERT_EQUAL_UINT32(cpu->PC, BIOS_OFFSET);
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

void test_function_ins_ori(void) {
	// ORI $t0, $t0, 0x0
	cpu->this_instruction = 0x35080000;
	instruction_Ori(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0 | 0);

	// ORI $t0, $t0, 0x0 (0x100 at $t0)
	cpu->registers[t0] = 0x100;
	cpu->this_instruction = 0x35080000;
	instruction_Ori(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x100 | 0);

	// ORI $t0, $t0, 0x100
	cpu->this_instruction = 0x35080100;
	instruction_Ori(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x0 | 0x100);

	// ORI $t0, $t0, 0x100 (0x100 at $t0)
	cpu->registers[t0] = 0x100;
	cpu->this_instruction = 0x35080100;
	instruction_Ori(cpu);
	TEST_ASSERT_EQUAL_UINT32(cpu->out_reg[t0], 0x100 | 0x100);

	// ORI $t0, $t0, 0xFFFF
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

int tests_Run() {
	UNITY_BEGIN();
	RUN_TEST(test_function_bios_address);
	// Logic Immediate
	RUN_TEST(test_function_ins_lui);
	// addi
	// addiu
	// andi
	RUN_TEST(test_function_ins_ori);
	// xori
	RUN_TEST(test_function_ins_sll);
	// srl
	// sra
	// slti
	// sltiu
	return UNITY_END();
}
