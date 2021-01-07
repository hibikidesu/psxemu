#ifndef PSX_CPU_HOOKS
#define PSX_CPU_HOOKS

#include "cpu.h"

//
// Hooks
//

// Debug prints
#define HOOK_TTY
// Log file read access (unimplemented)
#undef HOOK_FILE_OPEN
// Sideload EXE's without entering boot/cdrom load
#define HOOK_SIDELOAD

//
// Hook functions
//

// Hooks to putchar() and stores it in memory until a newline is hit.
void cpuHook_Putchar(CPU *cpu);
// Hooks all file access made by the cpu.
void cpuHook_FileOpen(CPU *cpu);
// Hooks SystemErrorUnresolvedException and causes the emulator to crash
// as it will hardlock if left unhooked.
void cpuHook_SystemErrorUnresolvedException(CPU *cpu);
// Loads an .exe into ram and executes it without needing to load from
// CDROM and without copy protection.
void cpuHook_SideLoad(CPU *cpu);
// Logs when a program is jumping to itself causing it to hang.
void cpuHook_SystemErrorExit(CPU *cpu);

#endif