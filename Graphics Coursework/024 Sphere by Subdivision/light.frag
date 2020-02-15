#version 440
// Point light structure

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

uniform directional_light lights_d[5];
uniform directional_light darks_d[5];
uniform point_light lights_p[32];
uniform point_light darks_p[32];
uniform vec3 light_count;
uniform vec3 dark_count;

vec4[2] calculate_direction(in directional_light light, in material mat, in vec3 normal, in vec3 view_dir, in vec4 tex_colour) {
	vec4 colour[2];

	vec4 ambient = mat.diffuse_reflection * light.ambient_intensity;

	float diffuse_multiplier = max(dot(normal, light.light_dir), 0.0);

	vec4 diffuse = diffuse_multiplier * (mat.diffuse_reflection * light.light_colour);

	vec3 half_vector = (light.light_dir + view_dir) / (length(light.light_dir + view_dir));

	float specular_multiplier = pow(max(dot(normal, half_vector), 0.0), mat.shininess);

	vec4 specular = (mat.specular_reflection * light.light_colour) * specular_multiplier;

	vec4 primary = diffuse + ambient;

	vec4 secondary = specular;

	// *************
	// Return colour
	// *************

	colour[0] = primary;
	colour[1] = secondary;
	return colour;
}

// point_light
vec4[2] calculate_point(in point_light light_in, in material mat, in vec3 normal, in vec3 position, in vec3 view_dir, in vec4 tex_colour){
	vec4 colour[2];
	colour[0] = vec4(0.0);
	colour[1] = vec4(0.0);
	float d = distance(light_in.position, position);
	vec4 light_colour = light_in.colour * (1.0 / (light_in.constant + (light_in.linear * d) + (light_in.quadratic * pow(d, 2))));
	vec3 light_dir = normalize(light_in.position - position);
	
	float diffuse_multiplier = max(dot(normal, light_dir), 0.0);
	vec4 diffuse = diffuse_multiplier * (mat.diffuse_reflection * light_colour);

	vec3 half_vector = normalize(light_dir + view_dir);

	float specular_multiplier = pow(max(dot(normal, half_vector), 0.0), mat.shininess);
	vec4 specular = (mat.specular_reflection * light_colour) * specular_multiplier;

	colour[0] = diffuse;
	colour[1] = specular;
	return colour;
}

// point_light
vec4[2] calculate_line(in point_light light, in material mat, in vec3 normal, in vec3 position, in vec3 view_dir, in vec3 eye_pos, in vec4 tex_colour){
	vec4 colour[2];
	colour[0] = vec4(0);
	colour[1] = vec4(0);
	vec3 normal_light_start = normalize(light.position - eye_pos);
	vec3 normal_light_end = normalize(eye_pos - light.position);
	vec3 normal_pos_start = position - eye_pos;
	vec3 normal_pos_end = position - light.position;

	vec3 light_pos;


	float dot_start = dot(normal_pos_start, normal_light_start);
	float dot_end = dot(normal_pos_end, normal_light_end);

	if (dot_start < 0.0) {
		light_pos = eye_pos;
	}

	else  if (dot_end < 0.0) {
		light_pos = light.position;
	}

	else {
		vec3 u = light.position - eye_pos;
		vec3 v = position - eye_pos;
		u = normalize(u);
		
		vec3 k = dot(u, v) * u;
		light_pos = k + eye_pos;
	}

	float d = distance(light_pos, position);
	vec4 light_colour = light.colour * (1.0f / (light.constant + (light.linear * d) + (light.quadratic * pow(d, 2))));
	vec3 light_dir = normalize(light_pos - position);

	float diffuse_multiplier = max(dot(normal, light_dir), 0.0);
	vec4 diffuse = diffuse_multiplier * (mat.diffuse_reflection * light_colour);

	vec3 half_vector = normalize(light_dir + view_dir);

	float specular_multiplier = pow(max(dot(normal, half_vector), 0.0), mat.shininess);
	vec4 specular = (mat.specular_reflection * light_colour) * specular_multiplier;

	vec4 primary = diffuse;

	colour[0] = primary;
	colour[1] = specular;
	return colour;
}

vec4 calculate_lighting(in material mat, in vec3 normal, in vec3 position, in vec3 eye_pos, in vec4 tex_colour){
	vec4 primary = vec4(0);
	vec4 secondary = vec4(0);
	vec4 lighting_colour[2];
	vec4 temp_tex_colour = tex_colour;
	temp_tex_colour.a = 1.0;

	vec3 view_dir = normalize(eye_pos - position);

	// First I set the intensity of primary and specular lighting at this point - depending on light/darklight sources

	for (int i = 0; i<light_count[0]; i++){
		lighting_colour = calculate_direction(lights_d[i], mat, normal, view_dir, temp_tex_colour);
		primary += lighting_colour[0];
		secondary += lighting_colour[1];
	}

	// Calculate point lights
	for (int i = 0; i<light_count[1]; i++){
		lighting_colour = calculate_point(lights_p[i], mat, normal, position, view_dir, temp_tex_colour);
		primary += lighting_colour[0];
		secondary += lighting_colour[1];
	}

	for (int i = 0; i<dark_count[0]; i++){
		lighting_colour = calculate_direction(darks_d[i], mat, normal, view_dir, temp_tex_colour);
		primary -= lighting_colour[0];
		secondary -= lighting_colour[0];
	}

	// Calculate point lights
	for (int i = 0; i<dark_count[1]; i++){
		lighting_colour = calculate_point(darks_p[i], mat, normal, position, view_dir, temp_tex_colour);
		primary -= lighting_colour[0];
		secondary -= lighting_colour[0];
	}

	primary.x = max(primary.x, 0.0);
	primary.y = max(primary.y, 0.0);
	primary.z = max(primary.z, 0.0);
	primary.a = max(primary.a, 0.0);

	primary += mat.emissive;

	secondary.x = max(secondary.x, 0.0);
	secondary.y = max(secondary.y, 0.0);
	secondary.z = max(secondary.z, 0.0);
	secondary.a = max(secondary.a, 0.0);

	vec4 out_colour = primary * temp_tex_colour + secondary;

	if (secondary.a < tex_colour.a){
		out_colour.a = tex_colour.a;
	}
	else{
		out_colour.a = clamp(secondary.a, 0.0, 1.0);
	}

	return out_colour;
}