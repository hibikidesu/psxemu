#include <stdlib.h>
#include <stdint.h>
#include "cdrom.h"

CDROM *cdrom_Create() {
	CDROM *cdrom = malloc(sizeof(CDROM));
	return cdrom;
}

void cdrom_Free(CDROM *cdrom) {
	free(cdrom);
}
