#pragma once
#include <glm\glm.hpp>
#include "scene_object.h"

using namespace graphics_framework;
using namespace glm;
using namespace std;

class post_processor {
private:
	static void load_effects(effect *screen_effect);
	static void load_frame_buffers();
	static int frame_current;
	static int motion_frame;
	static bool render_motion_blur;
	static frame_buffer temp_frame;
	static frame_buffer working_frames[2];
	static frame_buffer frame_previous[3];
	static scene_object screen;
	static texture tex_temp;
	static map<string, effect> effects;
	static void set_frame(const frame_buffer &fb, bool clear = true);
	static void set_frame(bool clear = true);
	static frame_buffer &working_frame_current(bool flip = false);
	static frame_buffer &working_frame_last();

public:
	static void initialise(effect *screen_effect);
	static void set_working_frame(texture tex_screen);
	static void set_working_frame(frame_buffer frame_screen);
	static texture get_working_frame();
	static void update();
	static void apply_invert();
	static void apply_contrast(float contrast_value);
	static void apply_cel();
	static void apply_edge();
	static void apply_greyscale();
	static void apply_sepia();
	static void apply_bloom(int intensity);
	static void apply_blend(texture frame_1, texture frame_2, float blend_factor_1, float blend_factor_2);
	static void apply_motion_blur();
	static void apply_rain(texture tex_rain, bool distort);
};