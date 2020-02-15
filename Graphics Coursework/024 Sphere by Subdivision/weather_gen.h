#pragma once

#include <graphics_framework.h>
#include "maths.h"


using namespace std;
using namespace glm;
using namespace graphics_framework;

class weather_gen{
public:
	static void addRain(const vec3 &point, const vec3 &cam_position, vector<vec3> &rain);
};