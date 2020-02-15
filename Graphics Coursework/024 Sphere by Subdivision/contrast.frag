#version 440
// Texture
uniform sampler2D frame_screen;

uniform float intensity;

// Incoming texture coordinate
layout (location = 10) in vec2 tex_coord;

// Outgoing colour
layout (location = 0) out vec4 colour;

void main()
{
	vec4 tex_colour = texture(frame_screen, tex_coord);

	tex_colour -= 0.5;
	tex_colour *= (1.0 + intensity);
	tex_colour += 0.5;

	tex_colour.a = 1.0;

	colour = tex_colour;
}