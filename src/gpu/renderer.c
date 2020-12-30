#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include "renderer.h"
#include "../utils/logger.h"

static SDL_Window *g_Window = NULL;
static SDL_GLContext *g_GlContext = NULL;
static GLuint vertex_shader;
static GLuint fragment_shader;
static GLuint program;
static uint32_t nvertices = 0;
static RendererColor *colorsBuffer[VERTEX_BUFFER_LEN];
static GLuint colorsObject;
static RendererPositon *positionsBuffer[VERTEX_BUFFER_LEN];
static GLuint positionsObject;
static GLuint vertex_arrays;

// void interpolate(RendererColor *color1, RendererColor *color2, float fraction) {
// 	uint8_t r = (uint8_t)((color2->r - color1->r) * fraction + color1->r);
// 	uint8_t g = (uint8_t)((color2->g - color1->g) * fraction + color1->g);
// 	uint8_t b = (uint8_t)((color2->b - color1->b) * fraction + color1->b);
// 	SDL_SetRenderDrawColor(g_Renderer, r, g, b, 255);
// }

// void algoDDA(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2) {
// 	float dx = (x2 - x1);
// 	float dy = (y2 - y1);
// 	float step;
// 	float x, y;
// 	int i;

// 	if (abs(dx) >= abs(dy)) {
// 		step = abs(dx);
// 	} else {
// 		step = abs(dy);
// 	}

// 	dx = (dx / step);
// 	dy = (dy / step);
// 	x = x1;
// 	y = y1;
// 	for (i = 0; i < step; i++) {
// 		SDL_RenderDrawPoint(g_Renderer, (int)x, (int)y);
// 		x += dx;
// 		y += dy;
// 	}
// }

void initBuffers() {
	glGenBuffers(1, &positionsObject);
	glBindBuffer(GL_ARRAY_BUFFER, positionsObject);

	// Compute size
	GLsizeiptr element_size = sizeof(RendererPositon);
	GLsizeiptr buffer_size = element_size * VERTEX_BUFFER_LEN;

	glBufferStorage(GL_ARRAY_BUFFER, buffer_size, NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
	*positionsBuffer = glMapBufferRange(GL_ARRAY_BUFFER, 0, buffer_size, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);

	// Colors buffer
	glGenBuffers(1, &colorsObject);
	glBindBuffer(GL_ARRAY_BUFFER, colorsObject);
	
	element_size = sizeof(RendererColor);
	buffer_size = element_size * VERTEX_BUFFER_LEN;

	glBufferStorage(GL_ARRAY_BUFFER, buffer_size, NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
	*colorsBuffer = glMapBufferRange(GL_ARRAY_BUFFER, 0, buffer_size, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
}

uint64_t readFile(char *filePath, char **contents) {
    uint64_t length = 0;
    FILE *file = fopen(filePath, "r");

    if (file) {
        // Check length
        fseek(file, 0, SEEK_END);
        length = ftell(file);

        // Back to beginning of file
        fseek(file, 0, SEEK_SET);
        *contents = malloc(sizeof(char) * length + 1);
        if (*contents != NULL) {
            fread(*contents, 1, length, file);
			(*contents)[length] = '\0';
        }
        fclose(file);
    }

    return length;
}

GLuint renderer_LoadShader(char *path, GLenum type) {
	char *data = NULL;
	readFile(path, &data);
	
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, (const char**)&data, NULL);
	glCompileShader(shader);

	GLint status = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		log_Error("Failed to compile shader %s", path);
		exit(1);
	}

	return shader;
}

void renderer_DrawTriangleShade(RendererPositon **positions, RendererColor **colors) {
	if ((nvertices + 3) > VERTEX_BUFFER_LEN) {
		log_Warn("Vertex Buffers Full, Forcing draw.");
		renderer_Update();
	}

	int i;
	for (i = 0; i < 3; i++) {
		positionsBuffer[nvertices] = positions[i];
		colorsBuffer[nvertices] = colors[i];
		nvertices += 1;
		log_Debug("ADD: X: %u Y: %u R: %u G: %u B: %u", positions[i]->x, positions[i]->y, colors[i]->r, colors[i]->g, colors[i]->b);
	}
}

RendererColor *renderer_GetColorFromGP0(uint32_t value) {
	RendererColor *color = malloc(sizeof(RendererColor));
	color->r = (uint8_t)value;
	color->g = (uint8_t)(value >> 8);
	color->b = (uint8_t)(value >> 16);
	return color;
}

RendererPositon *renderer_GetPositionFromGP0(uint32_t value) {
	RendererPositon *pos = malloc(sizeof(RendererPositon));
	pos->x = (int16_t)value;
	pos->y = (int16_t)(value >> 16);
	return pos;
}

void renderer_Update() {
	glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)nvertices);

	// Wait for GPU
	GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	int r;
	while (1) {
		r = glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, 10000000);
		if (r == GL_ALREADY_SIGNALED || r == GL_CONDITION_SATISFIED) {
			break;
		}
	}

	// Clear and swap window
	int i;
	for (i = 0; i < nvertices; i++) {
		log_Debug("BUFFER: X: %u Y: %u R: %u G: %u B: %u", positionsBuffer[i]->x, positionsBuffer[i]->y, colorsBuffer[i]->r, colorsBuffer[i]->g, colorsBuffer[i]->b);
		free(positionsBuffer[i]);
		free(colorsBuffer[i]);
	}
	nvertices = 0;

	SDL_GL_SwapWindow(g_Window);
}

void renderer_Init() {
	GLuint index;
	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		log_Error("Failed to init SDL2: %s", SDL_GetError());
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	g_Window = SDL_CreateWindow("uwu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 512, SDL_WINDOW_OPENGL);
	if (g_Window == NULL) {
		log_Error("Failed to create window: %s", SDL_GetError());
		exit(1);
	}

	g_GlContext = SDL_GL_CreateContext(g_Window);
	glewInit();

	// Wait for it to be ready
	SDL_Delay(80);

	// Load shaders
	vertex_shader = renderer_LoadShader("vertex.glsl", GL_VERTEX_SHADER);
	fragment_shader = renderer_LoadShader("fragment.glsl", GL_VERTEX_SHADER);

	// Create program
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glUseProgram(program);

	// Create vertex buffer
	vertex_arrays = 1;
	glGenVertexArrays(1, &vertex_arrays);
	glBindVertexArray(vertex_arrays);

	initBuffers();

	// Vertex attr
	index = glGetAttribLocation(program, "vertex_position");
	if (index < 0) {
		log_Error("Attrib vertex_position not found.");
		exit(1);
	}
	glEnableVertexAttribArray(index);
	glVertexAttribIPointer(index, 2, GL_SHORT, 0, NULL);

	// Colors attr
	index = glGetAttribLocation(program, "vertex_color");
	if (index < 0) {
		log_Error("Attrib vertex_color not found.");
		exit(1);
	}
	glEnableVertexAttribArray(index);
	glVertexAttribIPointer(index, 3, GL_UNSIGNED_BYTE, 0, NULL);

	renderer_Update();
}

void renderer_Destroy() {
	glDeleteVertexArrays(1, &vertex_arrays);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(program);
	SDL_GL_DeleteContext(g_GlContext);
	SDL_DestroyWindow(g_Window);
	SDL_Quit();
}
