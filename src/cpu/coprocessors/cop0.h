#ifndef PSX_COPROCESSORS_COP0
#define PSX_COPROCESSORS_COP0

#include "../cpu.h"
#define COP0 0b010000
#define LWC0 0b110000
#define SWC0 0b111000

void cop0_Handle(CPU *cpu);
void lwc0_Handle(CPU *cpu);
void swc0_Handle(CPU *cpu);

#endif