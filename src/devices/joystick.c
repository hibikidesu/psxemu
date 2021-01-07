#include <stdlib.h>
#include <stdint.h>
#include "joystick.h"
#include "../utils/logger.h"

void joystick_StoreShort(uint32_t offset, uint16_t value) {
	switch (offset) {
		case JOYSTICK_CTRL:
			log_Warn("Unimplemented JOY_CTRL value 0x%X", value);
			break;
		default:
			log_Error("Unknown Joystick %s offset 0x%X (0x%08X) value 0x%X", 
				__FUNCTION__, offset, offset + JOYSTICK_OFFSET, value);
			exit(1);
			break;
	}
}

uint16_t joystick_LoadShort(uint32_t offset) {
	uint16_t value = 0;

	switch (offset) {
		case JOYSTICK_CTRL:
			// log_Warn("Unimplemented JOY_CTRL LOAD value 0x%X", value);
			break;
		default:
			log_Error("Unknown Joystick %s offset 0x%X (0x%08X)", 
				__FUNCTION__, offset, offset + JOYSTICK_OFFSET);
			exit(1);
			break;
	}

	return value;
}

uint8_t joystick_LoadByte(uint32_t offset) {
	uint8_t value = 0;

	switch (offset) {
		case JOYSTICK_DATA:
			log_Warn("Unimplemented JOY_DATA");
			break;
		default:
			log_Error("Unknown Joystick %s offset 0x%X (0x%08X)", 
				__FUNCTION__, offset, offset + JOYSTICK_OFFSET);
			exit(1);
			break;
	}

	return value;
}
