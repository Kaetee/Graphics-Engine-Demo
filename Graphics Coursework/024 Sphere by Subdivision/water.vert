#version 440

// The model matrix
// The normal matrix
uniform mat3 N;

uniform float time;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 2) in vec3 normal;
// Incoming binormal
layout(location = 3) in vec3 binormal;
// Incoming tangent
layout(location = 4) in vec3 tangent;
// Incoming texture coordinates
layout(location = 10) in vec2 tex_coord_in;

// Outgoing position
layout(location = 0) out vec3 vertex_position;
// Outgoing normal
layout(location = 2) out vec3 normal_transformed;
// Incoming tangent
layout(location = 3) out vec3 binormal_transformed;
// Incoming binormal
layout(location = 4) out vec3 tangent_transformed;
// Outgoing texture coordinate
layout(location = 10) out vec2 tex_coord_out;

void main()
{
	// **************************************
	// Output other values to fragment shader
	// **************************************

	normal_transformed = N * normal;
	binormal_transformed = N * binormal;
	tangent_transformed = N * tangent;

	vertex_position = position;
	tex_coord_out = tex_coord_in;
}