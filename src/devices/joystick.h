#ifndef PSX_JOYSTICK
#define PSX_JOYSTICK

#include <stdint.h>

#define JOYSTICK_OFFSET 0x1F801040
#define JOYSTICK_SIZE 20

#define JOYSTICK_DATA 0x0
#define JOYSTICK_CTRL 0xA

void joystick_StoreShort(uint32_t offset, uint16_t value);
uint16_t joystick_LoadShort(uint32_t offset);
uint8_t joystick_LoadByte(uint32_t offset);

#endif