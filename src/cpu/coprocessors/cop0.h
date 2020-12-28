#ifndef PSX_COPROCESSORS_COP0
#define PSX_COPROCESSORS_COP0

#include "../cpu.h"
#define COP0 0b010000

void cop0_Handle(CPU *cpu);

#endif