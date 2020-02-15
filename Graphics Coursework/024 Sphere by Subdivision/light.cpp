#include "light.h"

// light class

light::light(){
	colour = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
	parent = NULL;
}

light::light(const glm::vec4 &colour){
	this->colour = colour;
	parent = NULL;
}

// directional light class

light_directional::light_directional() : light(){
	ambient = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	direction = glm::vec3(1.0f, 0.0f, 0.0f);
}

light_directional::light_directional(const glm::vec4 &colour, const glm::vec4 &ambient, const glm::vec3 &direction) : light(colour){
	this->ambient = ambient;
	this->direction = direction;
}

void light_directional::rotate(const glm::quat &rot){
	direction = glm::mat3_cast(rot) * direction;
}

void light_directional::rotate(const glm::vec3 &rot){
	rotate(glm::quat(rot));
}

// point light class

light_point::light_point() : light(){
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	constant = 0.5f;
	linear = 0.2f;
	quadratic = 0.01f;
}

light_point::light_point(const glm::vec4 &colour, const glm::vec3 &position, float constant, float linear, float quadratic) : light(colour){
	this->position = position;
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
}

light_point::light_point(const glm::vec4 &colour, const glm::vec3 &position, float constant, float range) : light(colour){
	this->position = position;
	this->constant = constant;
	this->linear = 2.0f / range;
	this->quadratic = (powf(range, 2.0f));
}

void light_point::set_range(float r){
	linear = 2.0f / r;
	quadratic = 1.0f / (powf(r, 2.0f));
}

void light_point::translate(const glm::vec3 &t){
	position += t;
}

// line light class

light_line::light_line() : light(){
	pos_start = glm::vec3(0.0f, 0.0f, 0.0f);
	pos_end = glm::vec3(1.0f, 1.0f, 1.0f);
	constant = 0.5f;
	linear = 0.2f;
	quadratic = 0.01f;
}

light_line::light_line(const glm::vec4 &colour, const glm::vec3 &pos_start, const glm::vec3 &pos_end, float constant, float linear, float quadratic) : light(colour){
	this->pos_start = pos_start;
	this->pos_end = pos_end;
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
}

light_line::light_line(const glm::vec4 &colour, const glm::vec3 &pos_start, const glm::vec3 &pos_end, float constant, float range) : light(colour){
	this->pos_start = pos_start;
	this->pos_end = pos_end;
	this->constant = constant;
	this->linear = 2.0f / range;
	this->quadratic = (powf(range, 2.0f));
}

void light_line::set_range(float r){
	linear = 2.0f / r;
	quadratic = 1.0f / (powf(r, 2.0f));
}

void light_line::translate(const glm::vec3 &t){
	pos_start += t;
	pos_end += t;
}

// spot light class

light_spot::light_spot() : light(){
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	direction = glm::vec3(0.0f, 0.0f, -1.0f);
	constant = 0.5f;
	linear = 0.2f;
	quadratic = 0.01f;
	power = 10.0f;
}

light_spot::light_spot(const glm::vec4 &colour, const glm::vec3 &position, const glm::vec3 &direction, float constant, float linear, float quadratic, float power) : light(colour){
	this->position = position;
	this->direction = direction;
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
	this->power = power;
}

light_spot::light_spot(const glm::vec4 &colour, const glm::vec3 &position, const glm::vec3 &direction, float constant, float range, float power)  : light(colour){
	this->position = position;
	this->direction = direction;
	this->constant = constant;
	this->linear = 2.0f / range;
	this->quadratic = (powf(range, 2.0f));
	this->power = power;
}

void light_spot::set_range(float r){
	linear = 2.0f / r;
	quadratic = 1.0f / (powf(r, 2.0f));
}

void light_spot::translate(const glm::vec3 &t){
	position += t;
}

void light_spot::rotate(const glm::quat &rot){
	direction = glm::mat3_cast(rot) * direction;
}

void light_spot::rotate(const glm::vec3 &rot){
	rotate(glm::quat(rot));
}