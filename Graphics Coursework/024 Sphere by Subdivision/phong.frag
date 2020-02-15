#version 440

// A directional light structure
struct directional_light
{
	vec4 ambient_intensity;
	vec4 light_colour;
	vec3 light_dir;
};

// A material structure
struct material
{
	vec4 emissive;
	vec4 diffuse_reflection;
	vec4 specular_reflection;
	float shininess;
};

// Directional light for the scene
uniform directional_light light;
// Material of the object
uniform material mat;
// Position of the camera
uniform vec3 eye_pos;
// Texture
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform int shader_type;

uniform int is_terrain;

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
	// ***************************
	// Calculate ambient component
	// ***************************
	vec4 ambient = mat.diffuse_reflection * light.ambient_intensity;
	float shininess_adjusted = 0.0f;
	
	vec3 normal = normalize(normal_input);

	// ***************************
	// Calculate diffuse component
	// ***************************max(dot(normalize(vTransformedNormal), lightDirection), 0.0);
    
	float diffuse_multiplier = max(dot(normal, light.light_dir), 0.0);
	
	// Calculate diffuse
	vec4 diffuse = diffuse_multiplier * (mat.diffuse_reflection * light.light_colour);
	

	// ************************
	// Calculate view direction
	// ************************
	vec3 view_dir = (eye_pos - position)/(length(eye_pos - position));
	

	// ****************************************************
	// Calculate half vector between view_dir and light_dir
	// ****************************************************
	vec3 half_vector = (light.light_dir + view_dir)/(length(light.light_dir + view_dir));

	// ****************************
	// Calculate specular component
	// ****************************
	float specular_multiplier = pow(max(dot(normal, half_vector), 0.0), mat.shininess);
	
	// Calculate specular
	vec4 specular = (mat.specular_reflection * light.light_colour) * specular_multiplier;
	

	// **************
	// Sample texture
	// **************
	vec4 tex_main;
	if(position[1] > 30){
		float alpha = clamp((position[1] - 30) / 5, 0.0, 1.0);
		
		vec4 tex_colour_1 = texture(tex1, tex_coord);
		vec4 tex_colour_3 = texture(tex3, tex_coord) + vec4(0.3f, 0.3f, 0.3f, 0.0f);

		tex_colour_3 *= alpha;
		tex_colour_1 *= 1.0f - alpha;

		shininess_adjusted = alpha;

		tex_main = tex_colour_1 + tex_colour_3;
	}
	else {
		tex_main = texture(tex1, tex_coord);
	}

	float output_angle = 0.0;
	vec3 Norm = normal / length(normal);
	float adjacent = sqrt(pow(0.0f, 2) + pow(Norm.y, 2) + pow(0.0f, 2));
	float hypotenuse = sqrt(pow(Norm.x, 2) + pow(Norm.y, 2) + pow(Norm.z, 2));
	output_angle = sqrt(pow(acos(adjacent/hypotenuse) * 180 / 3.141569, 2));

	vec4 tex_colour;
	if(output_angle > 0){
		float alpha = clamp((output_angle - 50) / 20, 0.0, 1.0);
		vec4 tex_colour_2 = texture(tex2, tex_coord);

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
	vec4 primary = mat.emissive + diffuse + ambient;
	

	// *************
	// Set secondary
	// *************
	vec4 secondary = shininess_adjusted * specular;
	

	// **********************
	// Calculate final colour
	// - remember alpha 1.0
	// **********************
	primary[3] = 1.0f;
	secondary[3] = 1.0f;

	colour = primary * tex_colour + secondary;	

}