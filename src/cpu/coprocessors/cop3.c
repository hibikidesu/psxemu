#include "cop3.h"
#include "../cpu.h"
#include "../../utils/logger.h"

void cop3_Handle(CPU *cpu) {
	cpu_Exception(cpu, EXCEPTION_COPERROR);
}

// Load Word Coprocessor 3
void lwc3_Handle(CPU *cpu) {
	cpu_Exception(cpu, EXCEPTION_COPERROR);
}

// Store Word Coprocessor 3
void swc3_Handle(CPU *cpu) {
	cpu_Exception(cpu, EXCEPTION_COPERROR);
}