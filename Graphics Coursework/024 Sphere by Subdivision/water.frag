#version 440

// A directional light structure
#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT
struct directional_light
{
	vec4 ambient_intensity;
	vec4 light_colour;
	vec3 light_dir;
};
#endif

#ifndef POINT_LIGHT
#define POINT_LIGHT
struct point_light
{
	vec4 colour;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
};
#endif

// A material structure
#ifndef MATERIAL
#define MATERIAL
struct material
{
	vec4 emissive;
	vec4 diffuse_reflection;
	vec4 specular_reflection;
	float shininess;
};
#endif

// Forward declarations of used functions
vec4 calculate_lighting(in material mat, in vec3 normal_new, in vec3 position, in vec3 eye_pos, in vec4 tex_colour);
vec3 calc_normal(in vec3 normal, in vec3 tangent, in vec3 binormal, in sampler2D normal_map, in vec2 tex_coord, in float texture_scale);

// Material of the object
uniform material mat;
// Position of the camera
uniform vec3 eye_pos;
// Texture
uniform sampler2D normal_map[2];

// Incoming position
layout (location = 0) in vec3 position;
// Incoming normal
layout (location = 2) in vec3 normal;
// Incoming tangent
layout (location = 3) in vec3 binormal;
// Incoming binormal
layout (location = 4) in vec3 tangent;
// Incoming texture coordinate
layout (location = 10) in vec2 tex_coord[2];

// Outgoing colour
layout (location = 0) out vec4 colour;

void main()
{
	vec3 view_dir = normalize(eye_pos - position);
	vec3 normal_fixed = normalize(normal);

	vec3 normal_new = 0.7 * calc_normal(normal_fixed, tangent, binormal, normal_map[0], tex_coord[0], 0.0);
	normal_new += 0.3 * calc_normal(normal_fixed, tangent, binormal, normal_map[1], tex_coord[1], 0.0);

	vec4 tex_colour = vec4(0.25, 0.64, 0.87, 0.4);
	

	vec4 out_colour = vec4(0.0);
	out_colour = calculate_lighting(mat, normal_new, position, eye_pos, tex_colour);
	

	// **********************
	// Calculate final colour
	// - remember alpha 1.0
	// **********************

	colour = out_colour;
}