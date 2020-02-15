#version 440
// Texture
uniform sampler2D frame_screen;

// Incoming texture coordinate
layout (location = 10) in vec2 tex_coords;

// returns pixel color
float GetEdge(in float threshold_1, in float threshold_2, in int range){
	vec4 tex_colour;
	float intensities[8];
	int k = 0;

	// read neighboring pixel intensities
	for (float i = -1; i<2; i += 1.0) {
		for (float j = -1; j<2; j += 1.0) {
			if (i == 0 && j == 0) continue;
			tex_colour = texture(frame_screen, tex_coords + vec2(i / (1920.0 / float(range)), j / (1080.0 / float(range))));
			intensities[k] = (tex_colour.r + tex_colour.g + tex_colour.b) / 3.0;
			k++;
		}
	}

	float average_intensity = 0;

	for (int i = 0; i < 4; i++){
		average_intensity += abs(intensities[i] - intensities[7 - i]);
	}

	average_intensity /= 4.0;

	average_intensity = clamp(2.0 * average_intensity, 0.0, 1.0);

	float edge_intensity;

	if (average_intensity < threshold_1)
		edge_intensity = 0.0;
	else if (average_intensity > threshold_2)
		edge_intensity = 1.0;
	else
		edge_intensity = average_intensity;

	return edge_intensity;
}