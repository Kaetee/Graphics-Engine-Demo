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

vec4[2] calculate_direction(in directional_light light, in material mat, in vec3 normal, in vec3 view_dir);
vec4[2] calculate_point(in point_light light, in material mat, in vec3 normal, in vec3 position, in vec3 view_dir);

// The transformation matrix
uniform mat4 MVP;

uniform mat4 lightMVP;
// The model transformation matrix
uniform mat4 M;
// The normal matrix
uniform mat3 N;

uniform directional_light lights_d[5];
uniform point_light lights_p[64];
uniform vec3 light_count;
// Material of the object being rendered
uniform material mat;
// Position of the eye
uniform vec3 eye_pos;

uniform bool lighting_type;
uniform bool rendering_shadow;

// Incoming position
layout (location = 0) in vec3 position;
// Incoming normal
layout (location = 2) in vec3 normal;
// Incoming binormal
layout (location = 3) in vec3 binormal;
// Incoming tangent
layout (location = 4) in vec3 tangent;
// Incoming texture coordinate
layout (location = 10) in vec2 tex_coord_in;

// Outgoing vertex position
layout (location = 0) out vec3 vertex_position;
// Outgoing normal
layout (location = 2) out vec3 normal_transformed;
// Outgoing tangent
layout (location = 3) out vec3 binormal_transformed;
// Outgoing binormal
layout (location = 4) out vec3 tangent_transformed;

layout (location = 5) out vec4 primary_out;

layout (location = 6) out vec4 secondary_out;

layout (location = 7) out vec4 vertex_light;
// Outgoing texture coordinate
layout (location = 10) out vec2 tex_coord_out;

void main()
{
    // Transform position into screen space
	gl_Position = MVP * vec4(position, 1.0);

	// Transform position into world space
	vertex_position = (M * vec4(position, 1.0)).xyz;

    // Pass through texture coordinate
	tex_coord_out = tex_coord_in;

	// Transform normal
	normal_transformed = N * normal;

	if (!rendering_shadow) {
		vertex_light = lightMVP * vec4(position, 1.0);
	}

	// If phong
	if (lighting_type == true){

		// Transform binormal
		binormal_transformed = N * binormal;

		// Transform tangent
		tangent_transformed = N * tangent;
	}

	// If gouraud
	else{
		vec3 view_dir = normalize(eye_pos - vertex_position);

		primary_out = vec4(0.0);
		secondary_out = vec4(0.0);

		vec4 lighting_colour[2];
		for (int i = 0; i<light_count[0]; i++){
			lighting_colour = calculate_direction(lights_d[i], mat, normal_transformed, view_dir);
			primary_out += lighting_colour[0];
			secondary_out += lighting_colour[1];
		}

		// Calculate point lights
		for (int i = 0; i<light_count[1]; i++){
			lighting_colour = calculate_point(lights_p[i], mat, normal_transformed, vertex_position, view_dir);
			primary_out += lighting_colour[0];
			secondary_out += lighting_colour[1];
		}
	}
}