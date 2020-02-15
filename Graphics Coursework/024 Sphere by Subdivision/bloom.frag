#version 440
// Texture
uniform sampler2D frame_screen;

uniform float cutoff;

// Incoming position
layout (location = 0) in vec3 position;

// Incoming texture coordinate
layout (location = 10) in vec2 tex_coord;

// Outgoing colour
layout (location = 0) out vec4 colour;

void main()
{
	vec4 tex_colour = texture(frame_screen, tex_coord);

	vec4 out_colour;

	if (tex_colour.r >= cutoff || tex_colour.g >= cutoff || tex_colour.b >= cutoff){
		// Change the range of (cutoff - 1.0) to (0.0 - 1.0)
		out_colour = tex_colour - vec4(cutoff / 2.0, cutoff / 2.0, cutoff / 2.0, 0.0);
	}
	else {
		// Output black
		out_colour = vec4(0.0);
	}


	out_colour.a = 1.0;
	colour = out_colour;
}