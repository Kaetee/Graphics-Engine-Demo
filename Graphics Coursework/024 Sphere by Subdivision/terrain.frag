#version 440

// This shader requires direction.frag and normal_map.frag

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

// Forward declarations of used functions
 vec4 calculate_lighting(in material mat, in vec3 normal, in vec3 position, in vec3 view_dir, in vec4 tex_colour);
 vec3 calc_normal(in vec3 normal, in vec3 tangent, in vec3 binormal, in sampler2D normal_map, in vec2 tex_coord, in float texture_scale);

// Material of the object being rendered
uniform material mat;
// Position of the eye
uniform vec3 eye_pos;

uniform int village_count;
uniform vec2 village_locations[15];
// Texture to sample from
uniform sampler2D tex[4];
// Normal map to sample from
uniform sampler2D normal_map[4];

uniform bool lighting_type;

// Incoming vertex position
layout (location = 0) in vec3 position;
// Incoming normal
layout (location = 2) in vec3 normal;
// Incoming tangent
layout (location = 3) in vec3 binormal;
// Incoming binormal
layout (location = 4) in vec3 tangent;

layout (location = 5) in vec4 in_primary;

layout (location = 6) in vec4 in_secondary;
// Incoming texture coordinate
layout (location = 10) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
	// **************
	// Sample texture
	// **************
	vec4 out_colour = vec4(0.0);

	float distance_from_eye = length(eye_pos - position);
	float texture_scale = 0.0;
	if (distance_from_eye > 400.0){
		texture_scale = 1.0f;
	}
	else if (distance_from_eye > 200.0){
		texture_scale = (distance_from_eye - 200.0) / 200.0;
	}

	vec4 tex_colour_1 = (((1.0 - texture_scale) * texture(tex[0], tex_coord)) + (texture_scale * texture(tex[0], tex_coord / 8.0)));
	vec4 tex_colour_2 = (((1.0 - texture_scale) * texture(tex[1], tex_coord)) + (texture_scale * texture(tex[1], tex_coord / 8.0)));
	vec4 tex_colour_3 = (((1.0 - texture_scale) * texture(tex[2], tex_coord)) + (texture_scale * texture(tex[2], tex_coord / 8.0)));
	vec4 tex_colour_4 = (((1.0 - texture_scale) * texture(tex[3], tex_coord)) + (texture_scale * texture(tex[3], tex_coord / 8.0)));
	tex_colour_4 *= vec4(0.88, 0.925, 1.1, 1.0);

	vec4 tex_colour;
	vec3 normal_new;
	float distance_to_village = 1000.0;
	float alpha_village_ground = 0.0;

	for (int i = 0; i < village_count; i++) {
		if (distance(village_locations[i], vec2(position.x, position.z)) < distance_to_village) {
			distance_to_village = distance(village_locations[i], vec2(position.x, position.z));
		}
	}

	if (distance_to_village < 120.0) {
		if (distance_to_village >= 100.0) {
			alpha_village_ground = 1.0 - ((distance_to_village - 100.0) / 20.0);
		}

		else {
			alpha_village_ground = 1.0;
		}
	}

	vec4 tex_main;
	float alpha_snow = 0.0;
	float alpha_grass = 0.0;
	float alpha_rock = 0.0;
	if (position[1] > 70){
		alpha_snow = clamp((position[1] - 70) / 10, 0.0, 1.0);
	}

	tex_main += tex_colour_1;

	float output_angle = 0.0;
	vec3 Norm = normal / length(normal);
	float adjacent = sqrt(pow(0.0f, 2) + pow(Norm.y, 2) + pow(0.0f, 2));
	float hypotenuse = sqrt(pow(Norm.x, 2) + pow(Norm.y, 2) + pow(Norm.z, 2));
	output_angle = sqrt(pow(acos(adjacent / hypotenuse) * 180 / 3.141569, 2));

	if (output_angle > 0){
		alpha_rock = clamp((output_angle - 40) / 20, 0.0, 1.0);
	}

	alpha_grass = 1.0f - clamp(alpha_rock + alpha_snow, 0.0, 1.0);

	if (alpha_rock + alpha_snow > 1.0){
		alpha_snow -= (alpha_rock + alpha_snow) - 1.0;
	}

	tex_colour_1 *= alpha_grass;
	tex_colour_2 *= alpha_rock;
	tex_colour_3 *= alpha_snow;
	tex_colour_4 *= alpha_village_ground;

	tex_colour = tex_colour_1 + tex_colour_2 + tex_colour_3;

	tex_colour *= 1.0 - alpha_village_ground;
	tex_colour += tex_colour_4;

	normal_new = alpha_grass * calc_normal(normal, tangent, binormal, normal_map[0], tex_coord, texture_scale);
	normal_new += alpha_rock * calc_normal(normal, tangent, binormal, normal_map[1], tex_coord, texture_scale);
	normal_new += alpha_snow * calc_normal(normal, tangent, binormal, normal_map[2], tex_coord, texture_scale);

	normal_new *= 1.0 - alpha_village_ground;
	normal_new += alpha_village_ground * calc_normal(normal, tangent, binormal, normal_map[3], tex_coord, texture_scale);

	// If phong
	if (lighting_type == true){

		// ************************
		// Calculate view direction
		// ************************
		vec3 view_dir = normalize(eye_pos - position);

		// ********************************
		// Calculate normal from normal map
		// ********************************
		out_colour = calculate_lighting(mat, normal_new, position, eye_pos, tex_colour);
	}

	// If gouraud
	else{
		out_colour = in_primary * tex_colour + in_secondary;
	}

	out_colour.a = 1.0;

	colour = out_colour;
}