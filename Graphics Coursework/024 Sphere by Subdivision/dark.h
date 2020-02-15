#pragma once
#include <glm\glm.hpp>
#include "scene_object.h"

// dark superclass
class dark{
private:
	glm::vec4 colour;
	scene_object* parent;

public:
	dark(const dark &other) = default;
	dark &operator=(const dark &rhs) = default;

	dark();
	dark(const glm::vec4 &colour);

	void set_colour(const glm::vec4 &c) { colour = c; }
	void set_parent(scene_object *o) { parent = o; }

	glm::vec4 get_colour() const { return colour; }
	scene_object *get_parent() const { return parent; }
};

// directional dark subclass
class dark_directional : public dark{
private:
	glm::vec4 ambient;
	glm::vec3 direction;

public:
	dark_directional(const dark_directional &other) = default;
	dark_directional &operator=(const dark_directional &rhs) = default;

	dark_directional();
	dark_directional(const glm::vec4 &colour, const glm::vec4 &ambient, const glm::vec3 &direction);

	void rotate(const glm::quat &rot);
	void rotate(const glm::vec3 &rot);

	void set_ambient_colour(const glm::vec4 &a) { ambient = a; }
	void set_direction(const glm::vec3 &dir) { direction = dir; }

	glm::vec4 get_ambient_colour() const { return ambient; }
	glm::vec3 get_direction() const { return direction; }
};

// point dark subclass
class dark_point : public dark{
private:
	glm::vec3 position;
	float constant;
	float linear;
	float quadratic;

public:
	dark_point(const dark_point &other) = default;
	dark_point &operator=(const dark_point &rhs) = default;

	dark_point();
	dark_point(const glm::vec4 &colour, const glm::vec3 &pos, float constant, float linear, float quadratic);
	dark_point(const glm::vec4 &colour, const glm::vec3 &pos, float constant, float range);

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

// spot dark subclass
class dark_spot : public dark{
private:
	glm::vec3 position;
	glm::vec3 direction;
	float power;
	float constant;
	float linear;
	float quadratic;

public:
	dark_spot(const dark_spot &other) = default;
	dark_spot &operator=(const dark_spot &rhs) = default;

	dark_spot();
	dark_spot(const glm::vec4 &colour, const glm::vec3 &position, const glm::vec3 &direction, float constant, float linear, float quadratic, float power);
	dark_spot(const glm::vec4 &colour, const glm::vec3 &position, const glm::vec3 &direction, float constant, float range, float power);

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