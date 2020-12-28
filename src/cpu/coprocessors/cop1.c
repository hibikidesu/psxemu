#include "cop1.h"
#include "../cpu.h"
#include "../../utils/logger.h"

void cop1_Handle(CPU *cpu) {
	cpu_Exception(cpu, EXCEPTION_COPERROR);
}

// Load Word Coprocessor 1
void lwc1_Handle(CPU *cpu) {
	cpu_Exception(cpu, EXCEPTION_COPERROR);
}

// Store Word Coprocessor 1
void swc1_Handle(CPU *cpu) {
	cpu_Exception(cpu, EXCEPTION_COPERROR);
}