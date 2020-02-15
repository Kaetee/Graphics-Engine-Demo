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
	vec4 grey = vec4(0.299, 0.587, 0.184, 0.0);
	
	float intensity = dot(tex_colour, grey);

	colour = vec4(intensity, intensity, intensity, 1.0);
}