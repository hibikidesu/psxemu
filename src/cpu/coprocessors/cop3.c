#include "cop3.h"
#include "../cpu.h"
#include "../../utils/logger.h"

void cop3_Handle(CPU *cpu) {
	cpu_Exception(cpu, EXCEPTION_COPERROR);
}