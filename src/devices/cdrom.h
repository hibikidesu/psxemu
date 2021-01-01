#ifndef PSX_CDROM
#define PSX_CDROM

#include <stdint.h>

#define CDROM_OFFSET 0x1F801800
#define CDROM_SIZE 3

typedef struct {

} CDROM;

CDROM *cdrom_Create();
void cdrom_Free(CDROM *cdrom);

#endif