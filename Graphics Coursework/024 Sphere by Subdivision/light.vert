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

// Calculates the directional light
vec4[2] calculate_direction(in directional_light light, in material mat, in vec3 normal, in vec3 view_dir)
{
	vec4 colour[2];

	vec4 ambient = mat.diffuse_reflection * light.ambient_intensity;

	float diffuse_multiplier = max(dot(normal, light.light_dir), 0.0);

	vec4 diffuse = diffuse_multiplier * (mat.diffuse_reflection * light.light_colour);

	vec3 half_vector = (light.light_dir + view_dir) / (length(light.light_dir + view_dir));

	float specular_multiplier = pow(max(dot(normal, half_vector), 0.0), mat.shininess);

	vec4 specular = (mat.specular_reflection * light.light_colour) * specular_multiplier;

	vec4 primary = mat.emissive + diffuse + ambient;

	vec4 secondary = specular;

	// *************
	// Return colour
	// *************

	colour[0] = primary;
	colour[1] = secondary;
	return colour;
}

// point_light
vec4[2] calculate_point(in point_light light, in material mat, in vec3 normal, in vec3 position, in vec3 view_dir){
	vec4 colour[2];
	float d = distance(light.position, position);
	vec4 light_colour = light.colour * (1.0f / (light.constant + (light.linear * d) + (light.quadratic * pow(d, 2))));
	vec3 light_dir = normalize(light.position - position);
	
	float diffuse_multiplier = max(dot(normal, light_colour), 0.0);
	vec4 diffuse = diffuse_multiplier * (mat.diffuse_reflection * light_colour);

	vec3 half_vector = normalize(light_dir + view_dir);

	float specular_multiplier = pow(max(dot(normal, half_vector), 0.0), mat.shininess);
	vec4 specular = (mat.specular_reflection * light_colour) * specular_multiplier;

	vec4 primary = mat.emissive + diffuse;

	colour[0] = primary;
	colour[1] = specular;
	return colour;
}