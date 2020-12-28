#include "cop1.h"
#include "../cpu.h"
#include "../../utils/logger.h"

void cop1_Handle(CPU *cpu) {
	cpu_Exception(cpu, EXCEPTION_COPERROR);
}