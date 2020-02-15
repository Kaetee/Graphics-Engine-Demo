#include "weather_gen.h"


void weather_gen::addRain(const vec3 &point, const vec3 &cam_position, vector<vec3> &rain){
	rain.push_back(point + cam_position);
	rain.push_back(vec3(point.x, point.y - 0.2f, point.z) + cam_position);
}