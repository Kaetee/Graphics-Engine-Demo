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
 vec4 calculate_direction(in directional_light light, in material mat, in vec3 normal, in vec3 view_dir, in vec4 tex_colour);
 vec3 calc_normal(in vec3 normal, in vec3 tangent, in vec3 binormal, in sampler2D normal_map, in vec2 tex_coord, in float texture_scale);

// Direction light being used in the scene
uniform directional_light light;
// Material of the object being rendered
uniform material mat;
// Position of the eye
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex_grass;
uniform sampler2D tex_snow;
uniform sampler2D tex_rock;
// Normal map to sample from
uniform sampler2D snow_normal_map;

// Incoming vertex position
layout (location = 0) in vec3 position;
// Incoming texture coordinate
layout (location = 2) in vec2 tex_coord;
// Incoming normal
layout (location = 1) in vec3 normal;
// Incoming tangent
layout (location = 3) in vec3 tangent;
// Incoming binormal
layout (location = 4) in vec3 binormal;

// Outgoing colour
layout (location = 0) out vec4 colour;

void main()
{
	colour = vec4(0.0, 0.0, 0.0, 1.0);
    // **************
    // Sample texture
    // **************

	float distance_from_eye = length(eye_pos - position);
	float texture_scale = 0.0f;
	vec2 tex_coord;
	if (distance_from_eye > 400.0f){
		texture_scale = 1.0f;
	}else if(distance_from_eye > 200.0f){
		texture_scale = (distance_from_eye - 200.0f) / 200.0f;
	}

	vec4 tex_main = texture(tex_snow, tex_coord);

	float output_angle = 0.0;
	vec3 Norm = normal / length(normal);
	float adjacent = sqrt(pow(0.0f, 2) + pow(Norm.y, 2) + pow(0.0f, 2));
	float hypotenuse = sqrt(pow(Norm.x, 2) + pow(Norm.y, 2) + pow(Norm.z, 2));
	output_angle = sqrt(pow(acos(adjacent / hypotenuse) * 180 / 3.141569, 2));

	vec4 tex_colour = tex_main;
	tex_colour[3] = 1.0f;
    
	// ************************
    // Calculate view direction
    // ************************
	vec3 view_dir = normalize(eye_pos - position);
    
	// ********************************
	// Calculate normal from normal map
	// ********************************
	vec3 normal_new = calc_normal(normal, tangent, binormal, snow_normal_map, tex_coord, texture_scale);

	//vec3 normal_new = normal;
	
    // ***************************
    // Calculate directional light
    // ***************************
	vec4 out_colour = calculate_direction(light, mat, normal_new, view_dir, tex_colour);

	colour = out_colour;
}