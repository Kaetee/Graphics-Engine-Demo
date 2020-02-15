#pragma once

#include <glm\glm.hpp>
#include <graphics_framework.h>
#include <thread>
#include "maths.h"

using namespace glm;
using namespace std;

class world_gen{
public:
	static void generate_seeds(int repeatAmount, vector<float> &height_map, vector<int> &direction_map, int width, int depth);

	static void expand_mount_child(vector<float> &height_map, float start_x, float start_y, int width, int depth);

	static void generate_seeds_mount(int repeatAmount, vector<float> &height_map, int width, int depth);

	static void generate_land_dir(const vector<float> &height_map, vector<int> &direction_map, int width, int depth);

	static void generate_continents_line(const vector<float> &height_map, vector<float> &row_height_map, vector<int> &direction_map, float height_mult, int row, int depth, int ecNormal, int ecSpecial);

	static void generate_land(int repeatAmount, vector<float> &height_map, vector<int> &direction_map, int width, int depth, int ecNormal, int ecSpecial);

	static void generate_hills(float repeatAmount, vector<float> &height_map, vector<int> &direction_map, int width, int depth, int ecNormal, int ecSpecial);

	static void smooth(int repeatAmount, vector<float> &height_map, int width, int depth, int smoothing_range);

	static void generate_plateau(int repeatAmount, vector<float> &height_map, int start_x, int start_y, int width, int depth);

	static void generate_mountains_line(vector<float> &height_map, vector<float> &row_height_map, int height_mult, int row, int width, int depth, int ecNormal, int ecSpecial);

	static void generate_mountains(int repeatAmount, vector<float> &height_map, int width, int depth, int ecNormal, int ecSpecial);

	static void join(vector<float> &height_map_1, vector<float> &height_map_2, int width, int depth);

	static void join_max(vector<float> &height_map_1, vector<float> &height_map_2, int width, int depth);

	static void stretch(vector<float> &height_map, int width, int depth);
};