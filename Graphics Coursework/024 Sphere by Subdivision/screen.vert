#version 440

// The transformation matrix
// Incoming position
layout (location = 0) in vec3 position;

// Incoming texture coordinates
layout (location = 10) in vec2 tex_coord_in;

// Outgoing position
layout (location = 0) out vec3 vertex_position;

// Outgoing texture coordinate
layout (location = 10) out vec2 tex_coord_out;

void main()
{
	// Set position
	gl_Position = vec4(position, 1.0);

	vertex_position = position;
	tex_coord_out = tex_coord_in;
}