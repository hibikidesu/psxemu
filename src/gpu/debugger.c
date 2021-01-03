#include <stdint.h>
#include <SDL2/SDL_gpu.h>
#include "debugger.h"

// Debugging overlay as couldnt get cimgui to work.

static uint8_t g_DebuggerEnabled = 0;

void gpudebugger_ToggleDebugger() {
	g_DebuggerEnabled ^= 1;
}

void gpudebugger_RenderFrame(GPU_Target *screen) {

}
