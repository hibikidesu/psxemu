#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cpu/cpu.h"
#include "devices/bios.h"
#include "devices/ram.h"
#include "devices/devices.h"
#include "devices/cdrom.h"
#include "gpu/gpu.h"
#include "utils/logger.h"
#include "tests.h"

int psx_Run(char *biosFile) {
	log_Info("Running psx");
	int status = 0;
	int i;

	// Create/Init devices
	CPU *cpu = cpu_Create();
	BIOS *bios = bios_Create(biosFile);
	RAM *ram = ram_Create();
	GPU *gpu = gpu_Create();
	CDROM *cdrom = cdrom_Create();
	DEVICES *devices = NULL;

	// Check if success opening bios
	if (bios == NULL) {
		cdrom_Free(cdrom);
		gpu_Destroy(gpu);
		ram_Destroy(ram);
		bios_Destroy(bios);
		cpu_Destroy(cpu);
		return 1;
	}

	// Add devices
	devices = devices_Create();
	devices_AddBios(devices, bios);
	devices_AddRAM(devices, ram);
	devices_AddGPU(devices, gpu);
	devices_AddCDROM(devices, cdrom);
	cpu_AddDevices(cpu, devices);

	// Run loop
	cpu->running = 1;
	while (cpu->running) {
		for (i = 0; i < 1000000; i++) {
			cpu_NextInstruction(cpu);
		}
		cpu_HandleEvents(cpu);
	}

	// Cleanup
	cdrom_Free(cdrom);
	gpu_Destroy(gpu);
	ram_Destroy(ram);
	devices_Destroy(devices);
	bios_Destroy(bios);
	cpu_Destroy(cpu);
	return status;
}

int main(int argc, char *argv[]) {
	int r = 0;
	if (argc > 1) {
		if (strcmp(argv[1], "test") == 0) {
			r = tests_Run();
		} else {
			r = psx_Run(argv[1]);
		}
	} else {
		r = psx_Run("data/SCPH1001.BIN");
	}
	return r;
}
