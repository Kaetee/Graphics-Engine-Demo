#version 440

// Cubemap texture
uniform samplerCube cubemap;
uniform float sun_theta;

// Incoming 3D texture coordinate
layout (location = 0) in vec3 tex_coord;

// Outgoing colour
layout (location = 0) out vec4 colour;

void main()
{
	// ************************
	// Sample texture as normal
	// ************************

	float lighting = (1.0 + sin(sun_theta / 180 * 3.141569)) / 4.0;
	lighting += 0.5;

	vec4 tex_colour = texture(cubemap, tex_coord);
	tex_colour *= lighting;

	colour = tex_colour;
}