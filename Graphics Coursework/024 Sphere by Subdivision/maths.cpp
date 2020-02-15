/*
This is the first file I write whenever I start a new project.
All my basic mathematical functions that can be a little tedious to write, and look quite ugly and clunky in my main code.
Everyone knows how they work, everyone uses them all the time, so might as well have a class to deal with the dirty code.
Functionality is specified in the header file
*/
#include "maths.h"
#include <random>

#define PI 3.14159265

// Returns a random value between the minimum and the maximum (inclusive)
int maths::random(int min, int max){
	std::default_random_engine e;
	e.seed(std::random_device()());
	std::uniform_int_distribution<int> dist(min, max);
	return dist(e);
}

bool maths::contains(vector<double> &input, double value) {
	for (double i : input) {
		if (i == value)
			return true;
	}
	return false;
}

bool maths::contains(vector<float> &input, float value) {
	for (float i : input) {
		if (i == value)
			return true;
	}
	return false;
}

bool maths::contains(vector<int> &input, int value) {
	for (int i : input) {
		if (i == value)
			return true;
	}
	return false;
}

double maths::product(vector<double> &input) {
	double output = 1.0;
	for (double i : input) {
		output *= i;
	}
	
	return output;
}

float maths::product(vector<float> &input) {
	float output = 1.0;
	for (float i : input) {
		output *= i;
	}
	
	return output;
}

int maths::product(vector<int> &input) {
	int output = 1;
	for (int i : input) {
		output *= i;
	}
	
	return output;
}

double maths::sum(vector<double> &input) {
	double output = 0.0;
	for (double i : input) {
		output += i;
	}
	
	return output;
}

float maths::sum(vector<float> &input) {
	float output = 0.0;
	for (float i : input) {
		output += i;
	}
	
	return output;
}

int maths::sum(vector<int> &input) {
	int output = 0;
	for (int i : input) {
		output += i;
	}
	
	return output;
}

double maths::to_radians(double input){
	return (input * (PI / 180.0));
}

float maths::to_radians(float input){
	return (input * (PI / 180.0));
}

double maths::to_degrees(double input){
	return (input * (180.0 / PI));
}

float maths::to_degrees(float input){
	return (input * (180.0 / PI));
}

double maths::max(double a, double b){
	return (a > b) ? a : b;
}

float maths::max(float a, float b){
	return (a > b) ? a : b;
}

double maths::min(double a, double b){
	return (a < b) ? a : b;
}

float maths::min(float a, float b){
	return (a < b) ? a : b;
}