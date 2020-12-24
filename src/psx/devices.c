#include <stdlib.h>
#include "devices.h"
#include "bios.h"
#include "ram.h"

void devices_AddRAM(DEVICES *devices, RAM *ram) {
	devices->ram = ram;
}

void devices_AddBios(DEVICES *devices, BIOS *bios) {
	devices->bios = bios;
}

DEVICES *devices_Create() {
	DEVICES *devices = malloc(sizeof(DEVICES));
	return devices;
}

void devices_Destroy(DEVICES *devices) {
	free(devices);
}
