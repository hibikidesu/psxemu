#ifndef GPU_DEBUGGER
#define GPU_DEBUGGER

#include <SDL2/SDL_gpu.h>

void gpudebugger_ToggleDebugger();
void gpudebugger_RenderFrame(GPU_Target *screen);

#endif