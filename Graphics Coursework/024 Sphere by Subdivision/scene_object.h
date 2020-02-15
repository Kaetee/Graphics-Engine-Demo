#pragma once

#include <graphics_framework.h>
#include <glm\glm.hpp>

class scene_object{
public:
	graphics_framework::mesh mesh;
	std::vector<graphics_framework::texture*> tex;
	std::vector<graphics_framework::texture*> norm;
	graphics_framework::effect *eff;
	float radius;
	std::vector<scene_object*> children;
	glm::mat4 transform;
	glm::mat3 normal_matrix;

	bool isInitialized;
	bool isInFrustum;

	scene_object(){
		isInitialized = false;
	}
};