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

// Calculates the directional light
vec4[2] calculate_direction(in directional_light light, in material mat, in vec3 normal, in vec3 view_dir, in vec4 tex_colour)
{
	vec4 colour[2];

	vec4 ambient = mat.diffuse_reflection * light.ambient_intensity;
    
	float diffuse_multiplier = max(dot(normal, light.light_dir), 0.0);
	
	vec4 diffuse = diffuse_multiplier * (mat.diffuse_reflection * light.light_colour);

	vec3 half_vector = (light.light_dir + view_dir)/(length(light.light_dir + view_dir));
    
	float specular_multiplier = pow(max(dot(normal, half_vector), 0.0), mat.shininess);
	
	vec4 specular = (mat.specular_reflection * light.light_colour) * specular_multiplier;
	
	vec4 primary = mat.emissive + diffuse + ambient;
	
	vec4 secondary = specular;

	// *************
	// Return colour
	// *************

	colour[0] = primary *  tex_colour;
	colour[1] = secondary;
	return colour;
}