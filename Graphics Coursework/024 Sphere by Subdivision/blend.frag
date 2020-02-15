#version 440
// Texture
uniform sampler2D frames[2];
uniform float blend_factor[2];

// Incoming position
layout (location = 0) in vec3 position;

// Incoming texture coordinate
layout (location = 10) in vec2 tex_coord;

// Outgoing colour
layout (location = 0) out vec4 colour;

void main()
{
	vec4 texture_colours[2];
	texture_colours[0] = texture(frames[0], tex_coord);
	texture_colours[1] = texture(frames[1], tex_coord);

	colour = (blend_factor[0] * texture_colours[0]) + (blend_factor[1] * texture_colours[1]);
	colour.a = 1.0;
}