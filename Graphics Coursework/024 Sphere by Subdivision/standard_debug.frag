#version 440

uniform vec3 eye_pos;

// Incoming texture coordinate
layout (location = 0) in vec4 colour_in;

layout (location = 1) in vec3 position;

// Outgoing colour
layout (location = 0) out vec4 colour_out;

void main()
{
	float d = distance(eye_pos, position);

	vec4 distance_multiplier = vec4(d);
	distance_multiplier /= 100.0;
	distance_multiplier.a = 1.0;

	colour_out = colour_in;
}