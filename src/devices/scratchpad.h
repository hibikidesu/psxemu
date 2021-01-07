#ifndef PSX_SCRATCHPAD
#define PSX_SCRATCHPAD

#include <stdint.h>

#define SCRATCHPAD_OFFSET 0x1F800000
#define SCRATCHPAD_SIZE 0x400  // 1024 Bytes

typedef struct {
	uint8_t data[SCRATCHPAD_SIZE];
} SCRATCHPAD;

void scratchpad_StoreByte(SCRATCHPAD *scratchpad, uint32_t address, uint8_t value);
void scratchpad_StoreShort(SCRATCHPAD *scratchpad, uint32_t address, uint16_t value);
uint16_t scratchpad_LoadShort(SCRATCHPAD *scratchpad, uint32_t address);
uint8_t scratchpad_LoadByte(SCRATCHPAD *scratchpad, uint32_t address);
uint32_t scratchpad_LoadInt(SCRATCHPAD *scratchpad, uint32_t address);
void scratchpad_StoreInt(SCRATCHPAD *scratchpad, uint32_t address, uint32_t value);
void scratchpad_Reset(SCRATCHPAD *scratchpad);
SCRATCHPAD *scratchpad_Create();
void scratchpad_Destroy(SCRATCHPAD *scratchpad);

#endif