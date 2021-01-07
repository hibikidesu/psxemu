#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "hooks.h"
#include "cpu.h"
#include "../devices/ram.h"
#include "instructions.h"
#include "../utils/logger.h"

#define CHR_BUFFER_MAX 64
static char CHR_BUFFER[CHR_BUFFER_MAX];
static uint16_t CHR_BUFFER_SIZE = 0;

void cpuHook_FlushCharBuffer() {
	CHR_BUFFER[CHR_BUFFER_SIZE] = '\0';
	CHR_BUFFER_SIZE = 0;
	log_Log(stdout, "TTY", "%s", CHR_BUFFER);
}

void cpuHook_Putchar(CPU *cpu) {
#ifdef HOOK_TTY
	switch (cpu_GetRegister(cpu, 9)) {
		case 0x3c:
		case 0x3d:
			// Check for buffer overflow
			if (CHR_BUFFER_SIZE == CHR_BUFFER_MAX - 1) {
				// Force flush
				cpuHook_FlushCharBuffer();
			}

			// If newline, flush else append
			if (cpu_GetRegister(cpu, 4) == '\n') {
				cpuHook_FlushCharBuffer();
			} else {
				CHR_BUFFER[CHR_BUFFER_SIZE] = cpu_GetRegister(cpu, 4);
				CHR_BUFFER_SIZE += 1;
			}
			break;
		default:
			break;
	}
#endif
}

void cpuHook_FileOpen(CPU *cpu) {
#ifdef HOOK_FILE_OPEN
// todo
	uint32_t file_mode = 0;
	switch (cpu_GetRegister(cpu, 9))
	{
		case 0x0:
		case 0x32:
			file_mode = cpu_GetRegister(cpu, 4);
			log_Log(stdout, "FileOpen", "Mode:");
			break;
		default:
			break;
	}
#endif
}

void cpuHook_SystemErrorUnresolvedException(CPU *cpu) {
	switch (cpu_GetRegister(cpu, 9)) {
		case 0x40:
			log_Error("SystemErrorUnresolvedException");
			exit(1);
			break;
		default:
			break;
	}
}

void cpuHook_SideLoad(CPU *cpu) {
#ifdef HOOK_SIDELOAD
	if (cpu->PC == 0x80030000) {
		cpu->log_instructions = 1;
		ExeFile *exe = ram_LoadEXE(cpu->devices->ram, "hello.ps-exe");
		if (exe == NULL) {
			cpu->running = false;
			exit(1);
			return;
		}
		cpu->PC = mask_region(exe->initial_pc);
		cpu->NEXT_PC = cpu->PC + 4;
		log_Info("Loaded EXE PC: 0x%X, RAM: 0x%X", mask_region(exe->initial_pc), mask_region(exe->ram_destination));
		cpu_SetRegister(cpu, 28, exe->initial_gp);
		cpu_SetRegister(cpu, 29, exe->initial_spfp_base);
		cpu_SetRegister(cpu, 30, exe->initial_spfp_base);
		// cpu_SetRegister(cpu, 29, exe->initial_spfp_base + exe->initial_spfp_off);
		// cpu_SetRegister(cpu, 30, exe->initial_spfp_base + exe->initial_spfp_off);
		free(exe);
		ram_Dump(cpu->devices->ram, "ram.bin");
	}
#endif
}

void cpuHook_SystemErrorExit(CPU *cpu) {
	switch (cpu_GetRegister(cpu, 9)) {
		case 0x40:
			log_Error("SystemErrorExit");
			exit(1);
			break;
		default:
			break;
	}
}