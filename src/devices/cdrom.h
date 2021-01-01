#ifndef PSX_CDROM
#define PSX_CDROM

#include <stdint.h>

#define CDROM_OFFSET 0x1F801800
#define CDROM_SIZE 3
#define CDROM_GET_STATUS 0x0
#define CDROM_RESPONSE_FIFO 0x1
#define CDROM_DATA_FIFO 0x2
#define CDROM_REQUEST_REGISTER 0x3

typedef struct {
	uint8_t index;
} CDROM;

void cdrom_StoreByte(CDROM *cdrom, uint32_t offset, uint8_t value);
uint8_t cdrom_LoadByte(CDROM *cdrom, uint32_t offset);
void cdrom_Reset(CDROM *cdrom);
CDROM *cdrom_Create();
void cdrom_Free(CDROM *cdrom);

#endif