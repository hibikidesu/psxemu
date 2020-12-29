#ifndef PSX_COMMANDBUFFER
#define PSX_COMMANDBUFFER

#include <stdint.h>

typedef struct {
	// Longest possible command = GP0(0x3E), 12 Params
	uint32_t buffer[12];
	// Words stored in buffer
	uint8_t length;
} CommandBuffer;

uint32_t commandBuffer_GetValue(CommandBuffer *commandBuffer, uint8_t index);
void commandBuffer_Store(CommandBuffer *commandBuffer, uint32_t value);
void commandBuffer_Clear(CommandBuffer *commandBuffer);
CommandBuffer *commandBuffer_Create();
void commandBuffer_Destroy(CommandBuffer *commandBuffer);

#endif