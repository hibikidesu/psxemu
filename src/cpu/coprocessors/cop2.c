#include "cop2.h"
#include "../cpu.h"
#include "../../utils/logger.h"

void cop2_Handle(CPU *cpu) {
	cpu_Exception(cpu, EXCEPTION_COPERROR);
}