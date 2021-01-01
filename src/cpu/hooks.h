#ifndef PSX_CPU_HOOKS
#define PSX_CPU_HOOKS

#include "cpu.h"

//
// Debug hooks
//

// Debug prints
#define HOOK_TTY
// Log file read access (unimplemented)
#undef HOOK_FILE_OPEN
// Sideload EXE's without entering boot/cdrom load
#define HOOK_FASTBOOT

// Hook functions
void cpuHook_Putchar(CPU *cpu);
void cpuHook_FileOpen(CPU *cpu);
void cpuHook_SystemErrorUnresolvedException(CPU *cpu);

#endif