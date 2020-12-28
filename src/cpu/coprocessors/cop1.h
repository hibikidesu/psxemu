#ifndef PSX_COPROCESSORS_COP1
#define PSX_COPROCESSORS_COP1

#include "../cpu.h"
#define COP1 0b010001
#define LWC1 0b110001
#define SWC1 0b111001

void cop1_Handle(CPU *cpu);
void lwc1_Handle(CPU *cpu);
void swc1_Handle(CPU *cpu);

#endif