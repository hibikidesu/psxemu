#ifndef PSX_CHANNEL
#define PSX_CHANNEL

#include <stdint.h>
#include <stdbool.h>

typedef enum {
	toDevice = 0,
	toRam = 1
} ChannelDirection;

typedef enum {
	increment = 0,
	decrement = 1
} ChannelStep;

typedef enum {
	manual = 0,
	request = 1,
	linkedList = 2
} ChannelSync;

typedef struct {
	bool enable;
	ChannelDirection direction;
	ChannelStep step;
	bool chop;
	ChannelSync sync;
	uint8_t chop_dma_s;
	uint8_t chop_cpu_s;
	bool trigger;
	uint8_t dummy;
	uint16_t block_size;
	uint16_t block_count;
	// DMA Start Address
	uint32_t base;
} CHANNEL;

bool channel_IsActive(CHANNEL *channel);
uint32_t channel_GetBlockControl(CHANNEL *channel);
void channel_SetBlockControl(CHANNEL *channel, uint32_t value);
uint32_t channel_GetBase(CHANNEL *channel);
void channel_SetBase(CHANNEL *channel, uint32_t value);
uint32_t channel_GetControl(CHANNEL *channel);
void channel_SetControl(CHANNEL *channel, uint32_t value);
void channel_Reset(CHANNEL *channel);
CHANNEL *channel_Create();
void channel_Free(CHANNEL *channel);

#endif