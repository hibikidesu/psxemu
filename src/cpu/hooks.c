#include <stdio.h>
#include <stdint.h>
#include "hooks.h"
#include "cpu.h"
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