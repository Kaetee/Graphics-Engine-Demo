/*
This class randomly generates landmasses and mountains.
It's a c++ remake of my Java program from first year
*/
#include "world_gen.h"

// Generates the original seeds of the world
void world_gen::generate_seeds(int pass_count, vector<float> &height_map, vector<int> &direction_map, int map_size_x, int map_size_z){
	int chance_value;
	
	
	for (int i = 0; i < pass_count; i++){
		int x = maths::random(64, map_size_x - 64);
		int y = maths::random(64, map_size_z - 64);
		height_map[x + y * map_size_x] = 1;

		chance_value = maths::random(10, 24);
		float xDir = maths::random(0, 16) / 2 - 4;
		float yDir = maths::random(0, 16) / 2 - 4;
		xDir *= 2;
		yDir *= 2;
	}
}

// Creates a child mountain range from a parent position
void world_gen::expand_mount_child(vector<float> &height_map, float startX, float startY, int map_size_x, int map_size_z){
	vector<float> output;
	output.resize(map_size_x * map_size_z); // initialize empty mountain map
	int pass_count = maths::random(32, 32); // How many passes should this process take?
	int counter = 0;
	counter++;
	
	//
	float xDir = maths::random(0, 3) - 2;
	float yDir = maths::random(0, 3) - 2;
	
	// If building this child would put us out of the maps boundaries, return
	if (startX + startY * map_size_x > map_size_x * map_size_z || startX + startY * map_size_x < 0){
		return;
	}

	float lastHeight = height_map[startX + startY * map_size_x]; // The last height should use the parent height
	float lastChange = -0.2f; // The initial y_direction should be downwards (it's a child arm of a mountain)
	float lastDirX = xDir; // Sets the initial expansion directions
	float lastDirY = yDir;
	int temp = 0;
	float lastX = startX; // Sets the initial expanded-from direction
	float lastY = startY;

	for (float i = 1; i < pass_count + 1; i += 1.0f){
		// 20% chance of changing height-direction (growing/sinking mountain)
		temp = maths::random(0, 4);
		if (lastHeight + lastChange < 0.5){ // If the mountain is getting too small, ensure there will be a change
			temp = 0;
		}
		
		// If temp is 0 (20% it will be), change the height direction of expansion
		if (temp == 0){
			lastChange = -((float)maths::random(0, 10)) / 50.0f; // Create a new, random height direction to expand into
		}
		else{
			lastHeight += lastChange; // If no change has occurred, simply increment the height with the previous height change
		}
		
		// If the mountain is still too small after that change, end the expansion
		if (lastHeight + lastChange < 0.5){
			i = pass_count + 1;
			continue;
		}

		// 10% chance to change direction
		temp = maths::random(0, 10);
		if (temp == 0){
			float newDirX = ((float)maths::random(1, 2));
			float newDirY = ((float)maths::random(1, 2));

			if (lastDirX < 0){ // 
				lastDirX = -newDirX;
			}
			else{
				lastDirX = newDirX;
			}

			if (lastDirY < 0){
				lastDirY = -newDirY;
			}
			else{
				lastDirY = newDirY;
			}
		}
		
		// Increment the x-z coordinates with the direction the mountain is expanding in
		lastX += lastDirX;
		lastY += lastDirY;

		// End the loop if you reach the edge of the map
		if ((lastX > map_size_x - 1) || (lastY > map_size_z - 1) || (lastX < 0) || (lastY < 0)){
			i = pass_count + 1;
			continue;
		}

		if (lastHeight > 1.0f){
		}
		
		// Set the height at these coordinates to the specified
		output[(lastX)+(lastY * map_size_x)] = lastHeight;
	}
	
	// Add the new mountain arm to the heightmap
	for (int i = 0; i < height_map.size(); i++){
		height_map[i] += output[i];
	}
}

// Creates a few initial peaks from which mountain ranges later expand


void world_gen::generate_seeds_mount(int pass_count, vector<float> &height_map, int mapSizeX, int mapSizeY){
	int chance_value;
	for (int i = 0; i < pass_count; i++){
		int x = maths::random(64, mapSizeX - 64);
		int y = maths::random(64, mapSizeY - 64);
		height_map[x + y * mapSizeX] = 4.0f;

		chance_value = maths::random(32, 64);
		float xDir = maths::random(1, 4) - 2;
		float yDir = maths::random(1, 4) - 2;

		float lastHeight = height_map[x + y * mapSizeX];
		float lastChange = -0.2f;
		float lastDirX = xDir;
		float lastDirY = yDir;
		int temp = 0;
		float lastX = x;
		float lastY = y;
		for (float i = 1; i < chance_value + 1; i += 1.0f){
			temp = maths::random(0, 4);
			if (lastHeight + lastChange < 0.5){
				temp = 0;
			}
			if (temp == 0){
				lastChange = ((float)maths::random(0, 20) - 10.0f) / 50.0f;
				if (lastHeight + lastChange < 0.5){
					lastChange = sqrt(pow(lastChange, 2));
				}
			}
			else{
				lastHeight += lastChange;
			}

			// 20% chance to change direction
			temp = maths::random(0, 10);
			if (temp == 0){
				float newDirX = ((float)maths::random(1, 2));
				float newDirY = ((float)maths::random(1, 2));

				if (lastDirX < 0){
					lastDirX = -newDirX;
				}
				else{
					lastDirX = newDirX;
				}

				if (lastDirY < 0){
					lastDirY = -newDirY;
				}
				else{
					lastDirY = newDirY;
				}

				temp = maths::random(0, 2);
				temp = 0;

				if (temp == 0){
					expand_mount_child(height_map, lastX + lastDirX, lastY + lastDirY, mapSizeX, mapSizeY);
				}

				// lastDirX += maths::random(0, 32) / 8 - 2;
				// lastDirY += maths::random(0, 32) / 8 - 2;
			}
			else{
				temp = maths::random(0, 2);

				if (temp == 0){
					expand_mount_child(height_map, lastX + lastDirX, lastY + lastDirY, mapSizeX, mapSizeY);
				}
			}

			lastX += lastDirX;
			lastY += lastDirY;

			// End the loop if you reach the edge of the map
			if ((lastX > mapSizeX - 1) || (lastY > mapSizeY - 1) || (lastX < 0) || (lastY < 0)){
				i = chance_value + 1;
				continue;
			}

			if (lastHeight > 1.0f){
			}
			height_map[(lastX)+(lastY * mapSizeX)] = lastHeight;
		}
	}
}

void world_gen::generate_land_dir(const vector<float> &height_map, vector<int> &direction_map, int mapX, int mapY){
	int chance_value;
	for (int i = 0; i < mapX; i++) {
		for (int j = 0; j < mapY; j++) {
			if (height_map[i + (j * mapX)] == 1) {
				chance_value = maths::random(0, 4);
				if (chance_value == 0 || chance_value == 1 || chance_value == 2) {
					direction_map[i + (j * mapX)] = 0;
				}
				else {
					direction_map[i + (j * mapX)] = 1;
				}
			}
			else{
				direction_map[i + (j * mapX)] = 3;
			}
		}
	}
}

// Go through a row of the heightmap and generate land there
void world_gen::generate_continents_line(const vector<float> &height_map_full, vector<float> &height_map_row, vector<int> &direction_map, float height_mult, int row, int map_size_z, int ecNormal, int ecSpecial){
	int counter = 0;
	int vcount = 0;
	int hcount = 0;
	int dir[2];
	int yStart = 0;
	int yEnd = 0;
	int xStart = 0;
	int xEnd = 0;
	int chance_value;
	
	// Makes sure the program won't check out of bounds
	if (row == 0)
		yStart = 0;
	else
		yStart = -1;

	if (row == map_size_z - 1)
		yEnd = 1;
	else
		yEnd = 2;

	for (int cols = 0; cols < map_size_z; cols++){
		if (height_map_full[cols + (row * map_size_z)] > 0){
			continue;
		}
		counter = 0;
		vcount = 0;
		hcount = 0;
		dir[0] = 0;
		dir[1] = 0;

		// Makes sure the program won't check out of bounds
		if (cols == 0)
			xStart = 0;
		else
			xStart = -1;

		if (cols == map_size_z - 1)
			xEnd = 1;
		else
			xEnd = 2;


		for (int i = yStart; i < yEnd; i++) {
			for (int j = xStart; j < xEnd; j++) {
				if (i == 0 && j == 0) continue;


				if (height_map_full[cols + j + ((row + i) * map_size_z)] > 0) {
					if (i == 0)
						hcount++;
					else if (j == 0)
						vcount++;
					else
						counter++;
					if (direction_map[cols + j + ((row + i) * map_size_z)] < 3){
						dir[direction_map[cols + j + ((row + i) * map_size_z)]]++;
					}
				}
			}
		}

		if (dir[0] > dir[1]) {
			direction_map[cols + (row * map_size_z)] = 0;
		}
		else if (dir[1] > dir[0]) {
			direction_map[cols + (row * map_size_z)] = 1;
		}
		else {
			direction_map[cols + (row * map_size_z)] = maths::random(0, 1);
		}

		if (direction_map[cols + (row * map_size_z)] == 1) {
			if (hcount > 0) {
				chance_value = maths::random(0, ecSpecial - (hcount + counter + vcount) - 1);
				if (chance_value == 0 || chance_value == 1) {
					height_map_row[cols + (row * map_size_z)] = height_mult;
				}
			}
			else if (counter > 0) {
				chance_value = maths::random(0, ecNormal - counter - 1);
				if (chance_value == 0 || chance_value == 1) {
					height_map_row[cols + (row * map_size_z)] = height_mult;
				}
			}
		}
		else {
			if (vcount > 0) {
				chance_value = maths::random(0, ecSpecial - (hcount + counter + vcount) - 1);
				if (chance_value == 0 || chance_value == 1) {
					height_map_row[cols + (row * map_size_z)] = height_mult;
				}
			}
			else if (counter > 0) {
				chance_value = maths::random(0, ecNormal - counter - 1);
				if (chance_value == 0 || chance_value == 1) {
					height_map_row[cols + (row * map_size_z)] = height_mult;
				}
			}
		}

		if (height_map_row[cols + (row * map_size_z)] == 0)
			direction_map[cols + (row * map_size_z)] = 3;
	}
}

void world_gen::generate_land(int pass_count, vector<float> &height_map, vector<int> &direction_map, int map_size_x, int map_size_z, int ecNormal, int ecSpecial) {
	int counter = 0;
	int vcount = 0;
	int hcount = 0;
	int dir[2];
	int nextX = 1;
	int yStart = 0;
	int yEnd = 0;
	float height_mult;
	vector<float> mapOutput;
	mapOutput.resize(height_map.size());
	mapOutput = height_map;

	vector<thread> threads;
	threads.resize(16);

	cout << height_map.size() - 1 << " | " << (map_size_x - 1) + ((map_size_z - 1) * map_size_z) << endl;
	cout << direction_map.size() - 1 << " | " << (map_size_x - 1) + ((map_size_z - 1) * map_size_z) << endl;
	for (int x = 0; x < pass_count; x++) {
		cout << "Generating Landmass: " << to_string(x) << " \ " << to_string(pass_count - 1) << endl;
		float k = pass_count - x;
		height_mult = 1.5f;
		for (int rows = 0; rows < map_size_z / 16; rows++) {
			for (int t = 0; t < threads.size(); t++){
				threads[t] = thread(generate_continents_line, ref(height_map), ref(mapOutput), ref(direction_map), height_mult, (rows * 16) + t, map_size_z, ecNormal, ecSpecial);
			}
			for (int t = 0; t < threads.size(); t++){
				threads[t].join();
			}
		}

		height_map = mapOutput;
	}
}

void world_gen::generate_hills(float pass_count, vector<float> &height_map, vector<int> &direction_map, int map_size_x, int map_size_z, int ecNormal, int ecSpecial) {
	int counter = 0;
	int vcount = 0;
	int hcount = 0;
	int dir[2];
	int nextX = 1;
	int yStart = 0;
	int yEnd = 0;
	float height_mult;
	vector<float> mapOutput;
	mapOutput.resize(height_map.size());
	mapOutput = height_map;

	vector<thread> threads;
	threads.resize(16);

	cout << height_map.size() - 1 << " | " << (map_size_x - 1) + ((map_size_z - 1) * map_size_z) << endl;
	cout << direction_map.size() - 1 << " | " << (map_size_x - 1) + ((map_size_z - 1) * map_size_z) << endl;
	for (float x = 0; x < pass_count; x += 1) {
		cout << "Generating Hills: " << to_string(x) << " \ " << to_string(pass_count - 1) << endl;
		float k = (pass_count - x) / pass_count;
		cout << "k: " << k << endl;
		k *= 15.0f;
		height_mult = k + 2.0f;
		for (int rows = 0; rows < map_size_z / 16; rows++) {
			for (int t = 0; t < threads.size(); t++){
				threads[t] = thread(generate_continents_line, ref(height_map), ref(mapOutput), ref(direction_map), height_mult, (rows * 16) + t, map_size_z, ecNormal, ecSpecial);
			}
			for (int t = 0; t < threads.size(); t++){
				threads[t].join();
			}
		}

		height_map = mapOutput;
	}
}

// Smoothing_Range - amount of adjacent vertices to check.
// 1 = 8: 3x3 square around current co-ordinate (check 1 above/below, 1 left/right, ignore centre)
// 2 = 24: 5x5 square around current co-ordinate (check 2 above/below, 2 left/right, ignore centre)
// 3 = 48: 7x7 square around current co-ordinate (check 3 above/below, 3 left/right, ignore centre)
// etc
void world_gen::smooth(int pass_count, vector<float> &height_map, int map_size_x, int map_size_z, int smoothing_range){
	float heights;

	int yStart = 0;
	int yEnd = 0;
	int xStart = 0;
	int xEnd = 0;
	int adjacency_count = pow((float)(smoothing_range * 2 + 1), 2) - 1; // Get size of sample square (-1 for centre)

	vector<float> output;
	output.resize(map_size_x * map_size_z);

	for (int x = 0; x < pass_count; x++) {
		cout << "Smoothing: " << to_string(x) << "/" << to_string(pass_count - 1) << endl;
		for (int rows = 0; rows < map_size_z; rows++) {

			// Set size of sample square around each co-ordinate (y-size)
			if (rows < smoothing_range){
				yStart = -rows;
			}
			else {
				yStart = -smoothing_range;
			}

			if (rows >= (map_size_z - smoothing_range)){
				yEnd = (map_size_z - rows);
			}
			else{
				yEnd = smoothing_range + 1;
			}

			for (int cols = 0; cols < map_size_x; cols++) {

				heights = 0;

				// Set size of sample square around each co-ordinate (x-size)
				if (cols < smoothing_range){
					xStart = -cols;
				}
				else{
					xStart = -smoothing_range;
				}

				if (cols >= (map_size_x - smoothing_range)){
					xEnd = (map_size_x - cols);
				}
				else{
					xEnd = smoothing_range + 1;
				}

				// Check surrounding vertices in specified range
				for (int i = yStart; i < yEnd; i++) {
					for (int j = xStart; j < xEnd; j++) {
						if (i == 0 && j == 0) continue; // Ignore centre (it's the current vertex)
						heights += height_map[cols + j + ((rows + i) * map_size_z)];
					}
				}

				heights /= adjacency_count;

				output[cols + (rows * map_size_x)] = (heights + height_map[cols + (rows * map_size_x)]) / 2;
			}
		}

		height_map = output;
	}
}

void world_gen::generate_plateau(int pass_count, vector<float> &height_map, int startX, int startY, int map_size_x, int map_size_z){
	vector<vec2> lands;
	lands.push_back(vec2(startX, startY));
	int chance_value;

	float height_level = height_map[startX + startY * map_size_x];

	for (int repeats = 0; repeats < pass_count; repeats++){
		int size = lands.size();
		for (int k = 0; k < size; k++){

			for (int i = -1; i < 2; i++){ // Check surrounding tiles
				for (int j = -1; j < 2; j++){
					if (i == 0 && j == 0){
						continue;
					}

					chance_value = maths::random(0, 4);
					if (chance_value == 0){
						if (!(std::find(lands.begin(), lands.end(), vec2(lands[k].x + i, lands[k].y + j)) != lands.end())){
							lands.push_back(vec2(lands[k].x + i, lands[k].y + j));
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < lands.size(); i++){
		if (lands[i].x < 0 || lands[i].x > map_size_x - 1 || lands[i].y < 0 || lands[i].y > map_size_z - 1){
			continue;
		}
		height_map[lands[i].x + lands[i].y * map_size_x] = maths::max(height_map[lands[i].x + lands[i].y * map_size_x], height_level);
	}
}

// Scans/expands the mountain heightmap on one row. For multi-threading
void world_gen::generate_mountains_line(vector<float> &height_map, vector<float> &mapOutput, int height_mult, int row, int map_size_x, int map_size_z, int ecNormal, int ecSpecial){
	int counter = 0;
	int surrounding_height;
	int yStart = 0;
	int yEnd = 0;
	int xStart = 0;
	int xEnd = 0;
	int chance_value; // Used to store the result of chance-based calculations


	if (row == 0)
		yStart = 0;
	else
		yStart = -1;

	if (row == map_size_z - 1)
		yEnd = 1;
	else
		yEnd = 2;

	for (int cols = 0; cols < map_size_x; cols++) {
		if (height_map[cols + (row * map_size_z)] > 0){
			int temp = maths::random(0, 2000);
			if (temp == 0 && row > 50 && row < map_size_x - 50 && cols > 50 && cols < map_size_z - 50){
				if (height_mult < 0.5f){
					generate_plateau(maths::random(4, 16), mapOutput, row, cols, map_size_x, map_size_z);
				}
			}
			continue;
		}

		if (cols == 0)
			xStart = 0;
		else
			xStart = -1;

		if (cols == map_size_x - 1)
			xEnd = 1;
		else
			xEnd = 2;

		counter = 0;

		for (int i = yStart; i < yEnd; i++) {
			for (int j = xStart; j < xEnd; j++) {
				if (i == 0 && j == 0) continue;
				if (height_map[cols + j + ((row + i) * map_size_z)] > 0) {
					counter++;
				}
			}
		}

		if (height_map[cols + (row * map_size_z)] == 0) {
			if (counter > 0) {
				chance_value = maths::random(0, ecNormal - (counter)-1);

				if (chance_value == 0 || chance_value == 1) {
					mapOutput[cols + (row * map_size_z)] = height_mult;
				}
			}
		}
	}
}

void world_gen::generate_mountains(int pass_count, vector<float> &height_map, int map_size_x, int map_size_z, int ecNormal, int ecSpecial) {
	float height_mult;
	vector<thread> threads;
	threads.resize(16);
	vector<float> mapOutput;
	mapOutput.resize(map_size_x * map_size_z);
	mapOutput = height_map;
	for (int x = 0; x < pass_count; x++) {
		cout << "Generating Mountains: " << to_string(x) << " \ " << to_string(pass_count - 1) << endl;
		float k = pass_count - x;
		height_mult = sqrt(pow(k, 3)) / pass_count;
		for (int rows = 0; rows < map_size_z / 16; rows++) {
			for (int t = 0; t < threads.size(); t++){
				threads[t] = thread(generate_mountains_line, ref(height_map), ref(mapOutput), height_mult, (rows * 16) + t, map_size_x, map_size_z, ecNormal, ecSpecial);
			}
			for (int t = 0; t < threads.size(); t++){
				threads[t].join();
			}
		}
		height_map = mapOutput;
	}
}

// Joins two heightmaps by simply adding their values together
void world_gen::join(vector<float> &height_map_1, vector<float> &height_map_2, int map_size_x, int map_size_z){
	for (int i = 0; i < map_size_x; i++){
		for (int j = 0; j < map_size_z; j++){
			height_map_2[j + i * map_size_x] += height_map_1[j + i * map_size_x];
		}
	}
}

// Joins two heightmaps by choosing the tallest point from each at every co-ordinate
void world_gen::join_max(vector<float> &height_map_1, vector<float> &height_map_2, int map_size_x, int map_size_z){
	for (int i = 0; i < map_size_x; i++){
		for (int j = 0; j < map_size_z; j++){
			float output = maths::max(height_map_2[j + i * map_size_x], height_map_1[j + i * map_size_x]);
			height_map_2[j + i * map_size_x] = output;
		}
	}
}

void world_gen::stretch(vector<float> &height_map, int width, int depth){
	for (int x = 0; x < width; x++){
		for (int z = 0; z < depth; z++){
			height_map[x + (z * width)] *= 10.0f;
		}
	}
}