#ifndef PSX_COPROCESSORS_COP1
#define PSX_COPROCESSORS_COP1

#include "../cpu.h"
#define COP1 0b010001

void cop1_Handle(CPU *cpu);

#endif