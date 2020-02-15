#version 440
// Texture
uniform sampler2D frame_screen;
uniform bool reduce_quality;

// Incoming position
layout(location = 0) in vec3 position;

// Incoming texture coordinate
layout(location = 10) in vec2 tex_coords;

// Outgoing colour
layout(location = 0) out vec4 colour;

float GetEdge(in float threshold_1, in float threshold_2, in int range);

vec4 GetClosestColor(in vec4 colour_in) {
	float nums[3];
	vec4 colour_out = vec4(1.0);

	for (int i = 0; i < 3; i++){
		nums[i] = colour_in[i] * 100.0;
		nums[i] = float(10 * (int(nums[i] + 5.0) / 10));

		colour_out[i] = nums[i] / 100.0;
	}

	return colour_out;
}

void main()
{
	float edge_intensity = GetEdge(0.3, 0.4, 2);

	vec4 out_colour;

	if (edge_intensity > 0.0) {
		out_colour = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else {
		out_colour = texture(frame_screen, tex_coords);
		if (reduce_quality)
			out_colour = GetClosestColor(out_colour);
	}

	colour = out_colour;
}