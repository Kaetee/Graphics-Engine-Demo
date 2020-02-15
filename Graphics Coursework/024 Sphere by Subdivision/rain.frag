#version 440

// Texture to sample from
uniform sampler2D tex_rain;

uniform vec4 rain_colour;

// Incoming texture coordinate
layout (location = 10) in vec2 tex_coord;

layout(location = 0) out vec4 out_colour;

void main()
{
	out_colour = texture(tex_rain, tex_coord);

	out_colour = vec4(1.0 - out_colour.r, 1.0 - out_colour.g, 1.0 - out_colour.b, 1.0);
}