#version 440
// Texture
uniform sampler2D frame_screen;
uniform bool reduce_quality;

// Incoming position
layout(location = 0) in vec3 position;

// Incoming texture coordinate
layout(location = 10) in vec2 tex_coords;

// Outgoing colour
layout(location = 0) out vec4 colour;

float GetEdge(in float threshold_1, in float threshold_2, in int range);

void main()
{
	float edge_intensity = GetEdge(0.3, 0.4, 2);

	colour = vec4(0.0, 0.0, edge_intensity, 1.0);
}