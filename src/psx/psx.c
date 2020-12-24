#include <stdio.h>
#include <stdint.h>
#include "psx.h"
#include "cpu.h"
#include "bios.h"
#include "devices.h"
#include "ram.h"
#include "../utils/logger.h"

int psx_Run(char *biosFile) {
	int status = 0;

	// Create/Init devices
	CPU *cpu = cpu_Create();
	BIOS *bios = bios_Create(biosFile);
	RAM *ram = ram_Create();
	DEVICES *devices = NULL;

	// Check if success opening bios
	if (bios == NULL) {
		ram_Destroy(ram);
		bios_Destroy(bios);
		cpu_Destroy(cpu);
		return 1;
	}

	// Add devices
	devices = devices_Create();
	devices_AddBios(devices, bios);
	devices_AddRAM(devices, ram);
	cpu_AddDevices(cpu, devices);

	// Run loop
	cpu->running = 1;
	while (cpu->running) {
		cpu_NextInstruction(cpu);
	}

	// Cleanup
	ram_Destroy(ram);
	devices_Destroy(devices);
	bios_Destroy(bios);
	cpu_Destroy(cpu);
	return status;
}
