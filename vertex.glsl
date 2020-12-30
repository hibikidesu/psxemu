#version 330 core

in ivec2 vertex_position;
in uvec3 vertex_color;

out vec3 color;

void main() {
	// Ram coords to opengl coords
	float xpos = (float(vertex_position.x) / 512) - 1.0;
	// VRAM 0 at top, opengl at bottom, mirror required
	float ypos = 1.0 - (float(vertex_position.y) / 256);

	gl_Position.xyzw = vec4(xpos, ypos, 0.0, 1.0);

	// Convert rgb to float
	color = vec3(float(vertex_color.r) / 255,
				 float(vertex_color.g) / 255,
				 float(vertex_color.b) / 255);
}