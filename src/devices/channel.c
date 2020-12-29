#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "channel.h"
#include "../utils/logger.h"

void channel_SetDone(CHANNEL *channel) {
	channel->enable = false;
	channel->trigger = false;
}

uint32_t channel_GetTransferSize(CHANNEL *channel) {
	uint32_t size = 0;
	uint32_t bs = (uint32_t)channel->block_size;
	uint32_t bc = (uint32_t)channel->block_count;

	switch (channel->sync) {
		case manual:
			size = bs;
			break;
		case request:
			size = bc * bs;
			break;
		default:
			break;
	}

	return size;
}

bool channel_IsActive(CHANNEL *channel) {
	bool trigger;
	if (channel->sync == manual) {
		trigger = channel->trigger;
	} else {
		trigger = true;
	}
	
	return channel->enable && trigger;
}

// Pack Block to Channel
uint32_t channel_GetBlockControl(CHANNEL *channel) {
	uint32_t bs = (uint32_t)channel->block_size;
	uint32_t bc = (uint32_t)channel->block_count;
	return (bc << 16) | bs;
}

// Unpack Block value in Channel
void channel_SetBlockControl(CHANNEL *channel, uint32_t value) {
	channel->block_size = (uint16_t)value;
	channel->block_count = (uint16_t)(value >> 16);
}

uint32_t channel_GetBase(CHANNEL *channel) {
	return channel->base;
}

void channel_SetBase(CHANNEL *channel, uint32_t value) {
	channel->base = value & 0xffffff;
}

uint32_t channel_GetControl(CHANNEL *channel) {
	uint32_t r = 0;
	r |= ((uint32_t)channel->direction) << 0;
	r |= ((uint32_t)channel->step) << 1;
	r |= ((uint32_t)channel->chop) << 8;
	r |= ((uint32_t)channel->sync) << 9;
	r |= ((uint32_t)channel->chop_dma_s) << 16;
	r |= ((uint32_t)channel->chop_cpu_s) << 20;
	r |= ((uint32_t)channel->enable) << 24;
	r |= ((uint32_t)channel->trigger) << 28;
	r |= ((uint32_t)channel->dummy) << 29;
	return r;
}

void channel_SetControl(CHANNEL *channel, uint32_t value) {
	// Set Direction
	if ((value & 1) != 0) {
		channel->direction = toDevice;
	} else {
		channel->direction = toRam;
	}

	// Set Step
	if (((value >> 1) & 1) != 0) {
		channel->step = decrement;
	} else {
		channel->step = increment;
	}

	// Set chop
	channel->chop = ((value >> 8) & 1) != 0;

	// Set Sync
	switch ((value >> 9) & 3) {
		case 0:
			channel->sync = manual;
			break;
		case 1:
			channel->sync = request;
			break;
		case 2:
			channel->sync = linkedList;
			break;
		default:
			log_Error("Unkown DMA sync mode 0x%X", (value >> 9) & 3);
			exit(1);
			break;
	}

	// Set window sizes
	channel->chop_dma_s = (uint8_t)((value >> 16) & 7);
	channel->chop_cpu_s = (uint8_t)((value >> 20) & 7);

	channel->enable = ((value >> 24) & 1) != 0;
	channel->trigger = ((value >> 28) & 1) != 0;

	channel->dummy = (uint8_t)((value >> 29) & 3);
}

void channel_Reset(CHANNEL *channel) {
	channel->enable = true;
	channel->direction = toRam;
	channel->step = increment;
	channel->sync = manual;
	channel->trigger = false;
	channel->chop = false;
	channel->chop_dma_s = 0;
	channel->chop_cpu_s = 0;
	channel->dummy = 0;
	channel->base = 0;
	channel->block_count = 0;
	channel->block_size = 0;
}

CHANNEL *channel_Create() {
	CHANNEL *channel = malloc(sizeof(CHANNEL));
	channel_Reset(channel);
	return channel;
}

void channel_Free(CHANNEL *channel) {
	free(channel);
}