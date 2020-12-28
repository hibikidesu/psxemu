#ifndef PSX_COPROCESSORS_COP3
#define PSX_COPROCESSORS_COP3

#include "../cpu.h"
#define COP3 0b010011
#define LWC3 0b110011
#define SWC3 0b111011

void cop3_Handle(CPU *cpu);
void lwc3_Handle(CPU *cpu);
void swc3_Handle(CPU *cpu);

#endif