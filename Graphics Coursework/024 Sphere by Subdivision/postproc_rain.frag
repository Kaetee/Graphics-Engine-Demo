#version 440

// Sampler used to get texture colour
uniform sampler2D tex_world;
uniform sampler2D tex_rain;
uniform vec4 colour_rain;

// Incoming texture coordinate
layout (location = 10) in vec2 tex_coord;
// Outgoing colour
layout (location = 0) out vec4 out_colour;

void main()
{
	// ****************************************
	// Set out colour to sampled texture colour
	// ****************************************
	vec2 final_tex_coord = tex_coord;
	vec4 colour_sampled = texture(tex_rain, tex_coord);

	final_tex_coord.x = final_tex_coord.x + (0.03 * colour_sampled.g);
	final_tex_coord.y = final_tex_coord.y + (0.03 * colour_sampled.g);
	out_colour = texture(tex_world, final_tex_coord) - (colour_rain * (colour_sampled.g / 16.0));

	out_colour.a = 1.0;
}