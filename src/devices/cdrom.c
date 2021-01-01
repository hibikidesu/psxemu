#include <stdlib.h>
#include <stdint.h>
#include "cdrom.h"
#include "../utils/logger.h"

uint8_t cdrom_GetStatus(CDROM *cdrom) {
	log_Warn("%s Unimplemented", __FUNCTION__);
	uint8_t r = cdrom->index;
	// ADPBUSY, set when playing XA-ADPCM sound
	r |= 0 << 2;
	// PRMEMPT, triggered before writing 1st byte
	r |= 1 << 3;
	// PRMWRDY, triggered after writing 16 bytes
	r |= 0 << 4;
	// RSLRRDY, triggered after reading LAST byte
	r |= 0 << 5;
	// DRQSTS, triggered after reading LAST byte
	r |= 0 << 6;
	// BUSYSTS, transmission busy
	r |= 0 << 7;
	return r;
}

uint8_t cdrom_InterruptEnableRegister(CDROM *cdrom) {
	return 0x1f;
}

uint8_t cdrom_DataFifo(CDROM *cdrom) {
	uint8_t r = 0;
	log_Warn("%s Unimplemented", __FUNCTION__);
	return r;
}

uint8_t cdrom_ResponseFifo(CDROM *cdrom) {
	uint8_t r = 0;
	log_Warn("%s Unimplemented", __FUNCTION__);
	return r;
}

void cdrom_StoreByte(CDROM *cdrom, uint32_t offset, uint8_t value) {
	switch (offset) {
		default:
			log_Error("%s unknown offset 0x%X (0x%X), value 0x%X", __FUNCTION__, offset, offset + CDROM_OFFSET, value);
			exit(1);
			break;
	}
}

uint8_t cdrom_LoadByte(CDROM *cdrom, uint32_t offset) {
	uint8_t value = 0;
	switch (offset) {
		case CDROM_GET_STATUS:
			value = cdrom_GetStatus(cdrom);
			break;
		case CDROM_RESPONSE_FIFO:
			value = cdrom_ResponseFifo(cdrom);
			break;
		case CDROM_DATA_FIFO:
			value = cdrom_DataFifo(cdrom);
			break;
		case CDROM_REQUEST_REGISTER:
			value = cdrom_InterruptEnableRegister(cdrom);
			break;
		default:
			log_Error("%s unknown offset 0x%X (0x%X)", __FUNCTION__, offset, offset + CDROM_OFFSET);
			exit(1);
			break;
	}
	return value;
}

void cdrom_Reset(CDROM *cdrom) {
	cdrom->index = 0;
}

CDROM *cdrom_Create() {
	CDROM *cdrom = malloc(sizeof(CDROM));
	return cdrom;
}

void cdrom_Free(CDROM *cdrom) {
	free(cdrom);
}
