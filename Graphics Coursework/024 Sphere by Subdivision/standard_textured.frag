#version 440

// Directional light structure
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

vec4 calculate_lighting(in material mat, in vec3 normal, in vec3 position, in vec3 view_dir, in vec4 tex_colour);
vec3 calc_normal(in vec3 normal, in vec3 tangent, in vec3 binormal, in sampler2D normal_map, in vec2 tex_coord, in float texture_scale);
// Position of the camera
uniform vec3 eye_pos;
// Texture
uniform sampler2D tex[1];
uniform sampler2D normal_map[1];
uniform material mat;

uniform bool lighting_type;
uniform bool has_normal_map;

// Incoming position
layout (location = 0) in vec3 position;
// Incoming normal
layout (location = 2) in vec3 normal_in;

layout (location = 3) in vec3 binormal_in;

layout (location = 4) in vec3 tangent_in;

layout(location = 5) in vec4 in_primary;

layout(location = 6) in vec4 in_secondary;

// Incoming texture coordinate
layout (location = 10) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
	vec4 tex_colour = texture(tex[0], tex_coord);
	vec4 out_colour = vec4(0.0);

	// If phong
	if (lighting_type == true){
		vec3 view_dir = normalize(eye_pos - position);

		vec3 normal = normalize(normal_in);
		vec3 binormal = normalize(binormal_in);
		vec3 tangent = normalize(tangent_in);

		vec3 normal_new;
		if (has_normal_map)
			normal_new = calc_normal(normal, tangent, binormal, normal_map[0], tex_coord, 0.0);
		else
			normal_new = normal;

		out_colour = calculate_lighting(mat, normal_new, position, eye_pos, tex_colour);
	}

	// If gouraud
	else{
		out_colour = in_primary * tex_colour + in_secondary;
	}

	colour = out_colour;
	colour.a = 1.0;
}