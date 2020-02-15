vec3 calc_normal(in vec3 normal, in vec3 tangent, in vec3 binormal, in sampler2D normal_map, in vec2 tex_coord, in float texture_scale)
{

	// ****************************************************************
	// Ensure normal, tangent and binormal are unit length (normalized)
	// ****************************************************************
	normal = normalize(normal);
	tangent = normalize(tangent);
	binormal = normalize(binormal);
	
	// *****************************
	// Sample normal from normal map
	// *****************************
	vec3 normal_sampled = (((1.0 - texture_scale) * texture(normal_map, tex_coord)) + (texture_scale * texture(normal_map, tex_coord / 8.0))).xyz;

	if (normal_sampled == vec3(0.0)) {
		return normal;
	}

	// ************************************
	// Transform components to range [0, 1]
	// ************************************
	normal_sampled = 2.0 * normal_sampled - (1.0, 1.0, 1.0);
	
	// *******************
	// Generate TBN matrix
	// *******************
	mat3 TBN = mat3(tangent, binormal, normal);
	
	// ****************************************
	// Return sampled normal transformed by TBN
	// - remember to normalize
	// ****************************************
	return normalize(TBN * normal_sampled);
}