#ifndef PSX_COPROCESSORS_COP2
#define PSX_COPROCESSORS_COP2

#include "../cpu.h"
#define COP2 0b010010
#define LWC2 0b110010
#define SWC2 0b111010

void cop2_Handle(CPU *cpu);
void lwc2_Handle(CPU *cpu);
void swc2_Handle(CPU *cpu);

#endif