#ifndef PSX_CPU_HOOKS
#define PSX_CPU_HOOKS

#include "cpu.h"

// Debug hooks
#undef HOOK_TTY
#undef HOOK_FILE_OPEN

// Hook functions
void cpuHook_Putchar(CPU *cpu);
void cpuHook_FileOpen(CPU *cpu);

#endif