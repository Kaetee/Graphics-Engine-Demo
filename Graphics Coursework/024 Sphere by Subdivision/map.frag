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

uniform mat4 M;

// Texture
uniform sampler2D tex[2];

uniform material mat;

// Incoming position
layout (location = 0) in vec3 position;
// Incoming normal
layout (location = 1) in vec3 normal_input;
// Incoming texture coordinate
layout (location = 2) in vec2 tex_coord;

// Outgoing colour
layout (location = 0) out vec4 colour;

void main()
{
	vec3 origin = (M * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
	if (position.y <= origin.y + 0.1){
		discard;
	}

	vec3 normal_new = normalize(normal_input);
	

	// ************************
	// Calculate view direction
	// ************************
	vec3 view_dir = normalize(eye_pos - position);
	

	// **************
	// Sample texture
	// **************
	vec4 tex_main = texture(tex[0], tex_coord);

	float output_angle = 0.0;
	vec3 Norm = normal_new / length(normal_new);
	float adjacent = sqrt(pow(0.0f, 2) + pow(Norm.y, 2) + pow(0.0f, 2));
	float hypotenuse = sqrt(pow(Norm.x, 2) + pow(Norm.y, 2) + pow(Norm.z, 2));
	output_angle = sqrt(pow(acos(adjacent/hypotenuse) * 180 / 3.141569, 2));

	vec4 tex_colour;
	if(output_angle > 0){
		float alpha = clamp((output_angle - 40) / 20, 0.0, 1.0);
		alpha *= pow(alpha, 2);

		vec4 tex_colour_2 = texture(tex[1], tex_coord);

		tex_main *= 1.0f - alpha;
		tex_colour_2 *= alpha;

		tex_colour = tex_main + tex_colour_2;
	}
	else{
		tex_colour = tex_main;
	}

	// **********************************
	// Calculate primary colour component
	// **********************************
	vec4 out_colour = calculate_lighting(mat, normal_new, position, eye_pos, tex_colour);

	colour = out_colour;
	colour.a = 1.0;
}