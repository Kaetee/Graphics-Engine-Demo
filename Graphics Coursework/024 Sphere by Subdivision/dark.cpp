#include "dark.h"

// dark class

dark::dark(){
	colour = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
	parent = NULL;
}

dark::dark(const glm::vec4 &colour){
	this->colour = colour;
	parent = NULL;
}

// directional dark class

dark_directional::dark_directional() : dark(){
	ambient = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	direction = glm::vec3(1.0f, 0.0f, 0.0f);
}

dark_directional::dark_directional(const glm::vec4 &colour, const glm::vec4 &ambient, const glm::vec3 &direction) : dark(colour){
	this->ambient = ambient;
	this->direction = direction;
}

void dark_directional::rotate(const glm::quat &rot){
	direction = glm::mat3_cast(rot) * direction;
}

void dark_directional::rotate(const glm::vec3 &rot){
	rotate(glm::quat(rot));
}

// point dark class

dark_point::dark_point() : dark(){
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	constant = 0.5f;
	linear = 0.2f;
	quadratic = 0.01f;
}

dark_point::dark_point(const glm::vec4 &colour, const glm::vec3 &position, float constant, float linear, float quadratic) : dark(colour){
	this->position = position;
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
}

dark_point::dark_point(const glm::vec4 &colour, const glm::vec3 &position, float constant, float range) : dark(colour){
	this->position = position;
	this->constant = constant;
	this->linear = 2.0f / range;
	this->quadratic = (powf(range, 2.0f));
}

void dark_point::set_range(float r){
	linear = 2.0f / r;
	quadratic = 1.0f / (powf(r, 2.0f));
}

void dark_point::translate(const glm::vec3 &t){
	position += t;
}

// spot dark class

dark_spot::dark_spot() : dark(){
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	direction = glm::vec3(0.0f, 0.0f, -1.0f);
	constant = 0.5f;
	linear = 0.2f;
	quadratic = 0.01f;
	power = 10.0f;
}

dark_spot::dark_spot(const glm::vec4 &colour, const glm::vec3 &position, const glm::vec3 &direction, float constant, float linear, float quadratic, float power) : dark(colour){
	this->position = position;
	this->direction = direction;
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
	this->power = power;
}

dark_spot::dark_spot(const glm::vec4 &colour, const glm::vec3 &position, const glm::vec3 &direction, float constant, float range, float power)  : dark(colour){
	this->position = position;
	this->direction = direction;
	this->constant = constant;
	this->linear = 2.0f / range;
	this->quadratic = (powf(range, 2.0f));
	this->power = power;
}

void dark_spot::set_range(float r){
	linear = 2.0f / r;
	quadratic = 1.0f / (powf(r, 2.0f));
}

void dark_spot::translate(const glm::vec3 &t){
	position += t;
}

void dark_spot::rotate(const glm::quat &rot){
	direction = glm::mat3_cast(rot) * direction;
}

void dark_spot::rotate(const glm::vec3 &rot){
	rotate(glm::quat(rot));
}