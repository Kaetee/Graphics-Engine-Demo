#version 440

// The transformation matrix
uniform mat4 MVP;
// The model transformation matrix
uniform mat4 M;

// Incoming position
layout (location = 0) in vec3 position;

// Outgoing vertex position
layout (location = 0) out vec3 vertex_position;

void main()
{
    // Transform position into screen space
	gl_Position = MVP * vec4(position, 1.0);

	// Transform position into world space
	vertex_position = (M * vec4(position, 1.0)).xyz;
}