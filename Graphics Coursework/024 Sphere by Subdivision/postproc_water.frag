#version 440

// Sampler used to get texture colour
uniform sampler2D tex_world;
uniform sampler2D tex_water;
uniform vec4 colour_rain;

uniform bool postproc_on;

// Incoming texture coordinate
layout (location = 0) in vec2 tex_coord;
// Outgoing colour
layout (location = 0) out vec4 out_colour;

void main()
{
	// ****************************************
	// Set out colour to sampled texture colour
	// ****************************************
	out_colour = texture(tex_world, tex_coord) + texture(tex_water, tex_coord);
}