#version 440

// Incoming position
layout (location = 0) in vec3 position;

// Outgoing vertex position
layout (location = 0) out vec3 vertex_position;

void main()
{

    // Transform position into world space
	vertex_position = position;
	
}