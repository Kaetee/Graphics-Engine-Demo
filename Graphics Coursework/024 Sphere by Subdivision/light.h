#pragma once
#include <glm\glm.hpp>
#include "scene_object.h"

// light superclass
class light{
private:
	glm::vec4 colour;
	scene_object* parent;

public:
	light(const light &other) = default;
	light &operator=(const light &rhs) = default;

	light();
	light(const glm::vec4 &colour);

	void set_colour(const glm::vec4 &c) { colour = c; }
	void set_parent(scene_object *o) { parent = o; }

	glm::vec4 get_colour() const { return colour; }
	scene_object *get_parent() const { return parent; }
};

// directional light subclass
class light_directional : public light{
private:
	glm::vec4 ambient;
	glm::vec3 direction;

public:
	light_directional(const light_directional &other) = default;
	light_directional &operator=(const light_directional &rhs) = default;

	light_directional();
	light_directional(const glm::vec4 &colour, const glm::vec4 &ambient, const glm::vec3 &direction);

	void rotate(const glm::quat &rot);
	void rotate(const glm::vec3 &rot);

	void set_ambient_colour(const glm::vec4 &a) { ambient = a; }
	void set_direction(const glm::vec3 &dir) { direction = dir; }

	glm::vec4 get_ambient_colour() const { return ambient; }
	glm::vec3 get_direction() const { return direction; }
};

// point light subclass
class light_point : public light{
private:
	glm::vec3 position;
	float constant;
	float linear;
	float quadratic;

public:
	light_point(const light_point &other) = default;
	light_point &operator=(const light_point &rhs) = default;

	light_point();
	light_point(const glm::vec4 &colour, const glm::vec3 &pos, float constant, float linear, float quadratic);
	light_point(const glm::vec4 &colour, const glm::vec3 &pos, float constant, float range);

	void set_range(float r);
	void translate(const glm::vec3 &t);

	void set_position(const glm::vec3 &pos) { position = pos; }
	void set_constant_attenuation(float value) { constant = value; }
	void set_linear_attenuation(float value) { linear = value; }
	void set_quadratic_attenuation(float value) { quadratic = value; }

	glm::vec3 get_position() const { return position; }
	float get_constant_attenuation() const { return constant; }
	float get_quadratic_attenuation() const { return quadratic; }
	float get_linear_attenuation() const { return linear; }
};

// line light subclass
class light_line : public light{
private:
	glm::vec3 pos_start;
	glm::vec3 pos_end;
	float constant;
	float linear;
	float quadratic;

public:
	light_line(const light_line &other) = default;
	light_line &operator=(const light_line &rhs) = default;

	light_line();
	light_line(const glm::vec4 &colour, const glm::vec3 &pos_start, const glm::vec3 &pos_end, float constant, float linear, float quadratic);
	light_line(const glm::vec4 &colour, const glm::vec3 &pos_start, const glm::vec3 &pos_end, float constant, float range);

	void set_range(float r);
	void translate(const glm::vec3 &t);

	void set_position(const glm::vec3 &pos1, const glm::vec3 &pos2) { pos_start = pos1; pos_end = pos2; }
	void set_constant_attenuation(float value) { constant = value; }
	void set_linear_attenuation(float value) { linear = value; }
	void set_quadratic_attenuation(float value) { quadratic = value; }

	glm::vec3 get_start_position() const { return pos_start; }
	glm::vec3 get_end_position() const { return pos_end; }
	float get_constant_attenuation() const { return constant; }
	float get_quadratic_attenuation() const { return quadratic; }
	float get_linear_attenuation() const { return linear; }
};

// spot light subclass
class light_spot : public light{
private:
	glm::vec3 position;
	glm::vec3 direction;
	float power;
	float constant;
	float linear;
	float quadratic;

public:
	light_spot(const light_spot &other) = default;
	light_spot &operator=(const light_spot &rhs) = default;

	light_spot();
	light_spot(const glm::vec4 &colour, const glm::vec3 &position, const glm::vec3 &direction, float constant, float linear, float quadratic, float power);
	light_spot(const glm::vec4 &colour, const glm::vec3 &position, const glm::vec3 &direction, float constant, float range, float power);

	void set_range(float r);
	void translate(const glm::vec3 &t);
	void rotate(const glm::quat &rot);
	void rotate(const glm::vec3 &rot);

	void set_position(const glm::vec3 &pos) { position = pos; }
	void set_direction(const glm::vec3 &dir) { direction = dir; }
	void set_power(float p) { power = p; }

	glm::vec3 get_position() const { return position; }
	glm::vec3 get_direction() const { return direction; }
	float get_power() const { return power; }
};