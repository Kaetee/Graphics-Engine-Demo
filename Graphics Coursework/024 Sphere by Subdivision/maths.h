#pragma once
#include <vector>

using namespace std;

class maths {
public:
	static int random(int min, int max); // gets a random value between the min and max (inclusive)
	static bool contains(vector<double> &input, double value); // checks if the input string contains the specified value
	static bool contains(vector<float> &input, float value);
	static bool contains(vector<int> &input, int value);
	static double product(vector<double> &input); // returns a product of all the array components
	static float product(vector<float> &input);
	static int product(vector<int> &input);
	static double sum(vector<double> &input); // returns a sum of all the array components
	static float sum(vector<float> &input);
	static int sum(vector<int> &input);
	static double to_radians(double input); // converts a specified angle in degrees to radians
	static float to_radians(float input);
	static double to_degrees(double input); // converts a specified angle in radians to degrees
	static float to_degrees(float input);
	static double max(double a, double b); // returns the larger value
	static float max(float a, float b);
	static double min(double a, double b); // returns the smaller value
	static float min(float a, float b);
};