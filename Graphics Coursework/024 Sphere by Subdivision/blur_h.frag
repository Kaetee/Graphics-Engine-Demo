#version 440
// Texture
uniform sampler2D frame_screen;

// Incoming position
layout (location = 0) in vec3 position;

// Incoming texture coordinate
layout (location = 10) in vec2 tex_coord;

// Outgoing colour
layout (location = 0) out vec4 colour;

void main()
{
	vec4 tex_colour = texture(frame_screen, tex_coord);
	
	float range = 15;
	
	vec4 colour_average = vec4(0.0);
	float intensity;
	for (float i = -range; i < (range + 1); i++) {
		intensity = 1.0 - (abs(i) / range);
		colour_average += intensity * texture(frame_screen, tex_coord + vec2(i / 1920, 0));
	}

	colour_average /= range * 2 + 1;

	colour_average /= colour_average.a;

	vec4 out_colour = colour_average;

	colour = out_colour;
}