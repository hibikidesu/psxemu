#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "commandbuffer.h"
#include "../utils/logger.h"

uint32_t commandBuffer_GetValue(CommandBuffer *commandBuffer, uint8_t index) {
	if (index >= commandBuffer->length) {
		log_Error("Tried to access an invalid CommandBuffer index");
		exit(1);
	}
	return commandBuffer->buffer[index];
}

void commandBuffer_Store(CommandBuffer *commandBuffer, uint32_t value) {
	assert(commandBuffer->length < 13);
	commandBuffer->buffer[commandBuffer->length] = value;
	commandBuffer->length += 1;
}

void commandBuffer_Clear(CommandBuffer *commandBuffer) {
	commandBuffer->length = 0;
}

CommandBuffer *commandBuffer_Create() {
	CommandBuffer *commandBuffer = malloc(sizeof(CommandBuffer));
	commandBuffer_Clear(commandBuffer);
	return commandBuffer;
}

void commandBuffer_Destroy(CommandBuffer *commandBuffer) {
	free(commandBuffer);
}