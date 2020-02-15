#include <ctime>
#include "scene_object.h"
#include <thread>
#include "light.h"
#include "dark.h"
#include "world_gen.h"
#include "weather_gen.h"
#include "maths.h"
#include "post_processor.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;

// Unchanging values
#define CAM_TURN_SPEED 0.0005f
#define CAM_MOVE_SPEED 2.0f
#define SIZE_MAP 256
#define SIZE_HEIGHTMAP 2048
#define MULTIPLIER_WIDTH SIZE_HEIGHTMAP/SIZE_MAP
#define SIZE_TILE 8.0f

// Cameras
free_camera cam;
target_camera sphere_cam;

// Light/dark arrays
vector<light_directional> lights_directional;
vector<light_point> lights_point;
vector<light_spot> lights_spot;
vector<dark_directional> darks_directional;
vector<dark_point> darks_point;
vector<dark_spot> darks_spot;

// input maps
map<string, bool> key_toggles; // Allows disabling of keys once pressed (for toggling functions)
map<string, unsigned int> key_bindings;
map<string, bool> graphics_toggles;

// Values used in code's logic
vector<float> heightmap_land;;
vector<vec3> rain; // Holds the rain positions
vector<vec2> village_locations;
vec3 wind_dir = vec3(1.0f, 0.0f, 0.0f); // Influences the rain fall direction
double mouseX;
double mouseY;
float contrast = 0.0f;
float dT; // A global time counter. Resets after a while so as not to overflow
float sun_theta = 0.0f;
float sun_distance = 1.0f;

// All maps used by objects
map<string, scene_object> objects;
map<string, effect> effects;
map<string, texture> textures;
map<string, texture> normal_maps;

// Frame buffers used in the main method
frame_buffer frame_default;
frame_buffer frame_rain;
frame_buffer frame_screen;
texture tex_screen;

// Typical shaders used in most effects
vector<string> shaders_vert{
	"light.vert",
};
vector<string> shaders_frag{
	"light.frag",
	"normal_map.frag",
};

map<string, vec3[2]> planes;

cubemap skybox_cube;
mesh skybox;
effect skybox_effect;

void init() {
	glfwGetCursorPos(renderer::get_window(), &mouseX, &mouseY);
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	key_toggles["map_on"] = false;
	key_toggles["phong_on"] = false;
	key_toggles["postproc::rain_on"] = false;
	key_toggles["postproc::bloom"] = false;
	key_toggles["postproc::greyscale"] = false;
	key_toggles["postproc::sepia"] = false;
	key_toggles["postproc::motion_blur"] = false;
	key_toggles["postproc::invert"] = false;
	key_toggles["postproc::cel"] = false;
	key_toggles["postproc::edge"] = false;
	key_toggles["contrast_up"] = false;
	key_toggles["contrast_down"] = false;
	key_toggles["weather::rain_on"] = false;
	key_toggles["weather::waves_on"] = false;
	key_toggles["sphere_cam_on"] = false;
	key_toggles["player_lamp_on"] = false;
	key_toggles["time::morning"] = false;
	key_toggles["time::noon"] = false;
	key_toggles["time::evening"] = false;
	key_toggles["time::midnight"] = false;
	key_bindings["map_on"] = GLFW_KEY_M;
	key_bindings["phong_on"] = GLFW_KEY_L;
	key_bindings["postproc::rain_on"] = GLFW_KEY_P;
	key_bindings["postproc::bloom"] = GLFW_KEY_B;
	key_bindings["postproc::greyscale"] = GLFW_KEY_G;
	key_bindings["postproc::sepia"] = GLFW_KEY_H;
	key_bindings["postproc::motion_blur"] = GLFW_KEY_N;
	key_bindings["postproc::invert"] = GLFW_KEY_V;
	key_bindings["postproc::cel"] = GLFW_KEY_Q;
	key_bindings["postproc::edge"] = GLFW_KEY_E;
	key_bindings["contrast_up"] = GLFW_KEY_X;
	key_bindings["contrast_down"] = GLFW_KEY_Z;
	key_bindings["weather::rain_on"] = GLFW_KEY_R;
	key_bindings["weather::waves_on"] = GLFW_KEY_K;
	key_bindings["sphere_cam_on"] = GLFW_KEY_I;
	key_bindings["player_lamp_on"] = GLFW_KEY_J;
	key_bindings["time::morning"] = GLFW_KEY_1;
	key_bindings["time::noon"] = GLFW_KEY_2;
	key_bindings["time::evening"] = GLFW_KEY_3;
	key_bindings["time::midnight"] = GLFW_KEY_4;
	graphics_toggles["postproc::rain_on"] = true;
	graphics_toggles["weather::rain_on"] = false;
	graphics_toggles["weather::waves_on"] = false;
	graphics_toggles["phong_on"] = true;
	graphics_toggles["sphere_cam_on"] = false;
	graphics_toggles["postproc::bloom"] = false;
	graphics_toggles["postproc::greyscale"] = false;
	graphics_toggles["postproc::sepia"] = false;
	graphics_toggles["postproc::motion_blur"] = false;
	graphics_toggles["postproc::invert"] = false;
	graphics_toggles["postproc::cel"] = false;
	graphics_toggles["postproc::edge"] = false;
	array <string, 6> filenames =
	{
		"cubemaps\\alien\\posx.png",
		"cubemaps\\alien\\negx.png",
		"cubemaps\\alien\\posy.png",
		"cubemaps\\alien\\negy.png",
		"cubemaps\\alien\\posz.png",
		"cubemaps\\alien\\negz.png"
	};

	skybox_cube = cubemap(filenames);
	geometry geom;

	geometry skygeom;
	skygeom.set_type(GL_QUADS);
	vector<vec3> positions
	{
		// Face 1
		vec3(-1.0f, 1.0f, 1.0f),
		vec3(1.0f, 1.0f, 1.0f),
		vec3(1.0f, -1.0f, 1.0f),
		vec3(-1.0f, -1.0f, 1.0f),

		// Face 2
		vec3(1.0f, 1.0f, 1.0f),
		vec3(1.0f, 1.0f, -1.0f),
		vec3(1.0f, -1.0f, -1.0f),
		vec3(1.0f, -1.0f, 1.0f),

		// Face 3
		vec3(-1.0f, 1.0f, -1.0f),
		vec3(1.0f, 1.0f, -1.0f),
		vec3(1.0f, 1.0f, 1.0f),
		vec3(-1.0f, 1.0f, 1.0f),

		// Face 4
		vec3(-1.0f, -1.0f, -1.0f),
		vec3(1.0f, -1.0f, -1.0f),
		vec3(1.0f, 1.0f, -1.0f),
		vec3(-1.0f, 1.0f, -1.0f),

		// Face 5
		vec3(-1.0f, -1.0f, 1.0f),
		vec3(-1.0f, -1.0f, -1.0f),
		vec3(-1.0f, 1.0f, -1.0f),
		vec3(-1.0f, 1.0f, 1.0f),

		// Face 6
		vec3(-1.0f, -1.0f, 1.0f),
		vec3(1.0f, -1.0f, 1.0f),
		vec3(1.0f, -1.0f, -1.0f),
		vec3(-1.0f, -1.0f, -1.0f),

	};
	skygeom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	skybox = mesh(skygeom);
	skybox.get_transform().scale = vec3(100.0f);

	skybox_effect.add_shader("skybox.vert", GL_VERTEX_SHADER);
	skybox_effect.add_shader("skybox.frag", GL_FRAGMENT_SHADER);
	skybox_effect.build();

	objects["world"] = scene_object();
	objects["screen"] = scene_object();
	objects["worldspace"] = scene_object();
	objects["worldshadow"] = scene_object();
	objects["weather"] = scene_object();

	objects["world"].children.push_back(&objects["screen"]);
	objects["world"].children.push_back(&objects["worldspace"]);
	objects["world"].children.push_back(&objects["worldshadow"]);
	objects["world"].children.push_back(&objects["weather"]);

	effects["standard_textured"].add_shader("standard_textured.vert", GL_VERTEX_SHADER);
	effects["standard_textured"].add_shader(shaders_vert, GL_VERTEX_SHADER);
	effects["standard_textured"].add_shader("standard_textured.frag", GL_FRAGMENT_SHADER);
	effects["standard_textured"].add_shader(shaders_frag, GL_FRAGMENT_SHADER);
	effects["standard_textured"].build();

	effects["sea"].add_shader("water.vert", GL_VERTEX_SHADER);
	effects["sea"].add_shader("water.geometry", GL_GEOMETRY_SHADER);
	effects["sea"].add_shader("water.frag", GL_FRAGMENT_SHADER);
	effects["sea"].add_shader(shaders_frag, GL_FRAGMENT_SHADER);
	effects["sea"].build();

	effects["map"].add_shader("map.vert", GL_VERTEX_SHADER);
	effects["map"].add_shader("map.geometry", GL_GEOMETRY_SHADER);
	effects["map"].add_shader("map.frag", GL_FRAGMENT_SHADER);
	effects["map"].add_shader(shaders_frag, GL_FRAGMENT_SHADER);
	effects["map"].build();

	effects["rain"].add_shader("rain.vert", GL_VERTEX_SHADER);
	effects["rain"].add_shader("rain.geometry", GL_GEOMETRY_SHADER);
	effects["rain"].add_shader("rain.frag", GL_FRAGMENT_SHADER);
	effects["rain"].build();

	effects["screen"].add_shader("screen.vert", GL_VERTEX_SHADER);
	effects["screen"].add_shader("output.frag", GL_FRAGMENT_SHADER);
	effects["screen"].build();

	effects["terrain"].add_shader("terrain.vert", GL_VERTEX_SHADER);
	effects["terrain"].add_shader(shaders_vert, GL_VERTEX_SHADER);
	effects["terrain"].add_shader("terrain.frag", GL_FRAGMENT_SHADER);
	effects["terrain"].add_shader(shaders_frag, GL_FRAGMENT_SHADER);
	effects["terrain"].build();
}

vec3 getNormal(vector<vec3> &input) {
	vec3 output;
	vec3 Dir = cross(input[1] - input[0], input[2] - input[0]);
	vec3 Norm = normalize(Dir);
	return Norm;
}

vector<float> generate_continents(int width_final, int depth_final) {
	int width = width_final;
	int depth = depth_final;
	vector<float> heights;
	vector<float> continents;
	vector<int> dirLand;
	vector<int> dirMount;
	continents.resize(width * depth);
	heights.resize(width * depth);
	dirLand.resize(width * depth);
	dirMount.resize(width * depth);

	world_gen::generate_seeds(32, continents, dirLand, width, depth);
	world_gen::generate_land_dir(continents, dirLand, width, depth);
	world_gen::generate_land(48, continents, dirLand, width, depth, 24, 9);
	world_gen::smooth(8, continents, width, depth, 1);

	world_gen::generate_seeds_mount(4, heights, width, depth);
	world_gen::generate_land_dir(heights, dirMount, width, depth);
	world_gen::generate_mountains(32, heights, width, depth, 14, 14);
	world_gen::smooth(2, heights, width, depth, 1);
	world_gen::join(heights, continents, width, depth);

	cout << "setting heights: DONE" << endl;

	vector<float> temp;
	temp.resize(width_final * depth_final);
	for (int x = 0; x < width; x++) {
		for (int z = 0; z < depth; z++) {
			temp[x + (z * width_final)] = continents[x + (z * depth)] * 10.0f;
		}
	}

	return temp;
}

geometry create_terrain(const unsigned int width, const unsigned int depth, vector<float> &hm, float scale) {
	// Type of geometry generated will be triangles
	geometry geom;
	// Declare required buffers - positions, normals, texture coordinates and colour
	vector<GLuint> indices;
	vector<vec3> positions;
	vector<vec3> normals;
	vector<vec3> binormals;
	vector<vec3> tangents;
	vector<vec2> tex_coords;
	vector<vec4> colours;
	vec3 minimal(0.0f, 0.0f, 0.0f);
	vec3 maximal(0.0f, 0.0f, 0.0f);
	float add_to_heightmap = false;
	if (hm == heightmap_land) {
		add_to_heightmap = true;
	}

	// Minimal and maximal points
	float size_multiplier = SIZE_HEIGHTMAP / (SIZE_MAP);

	positions.resize(width * depth);
	normals.resize(width * depth);

	for (int x = 0; x < width; ++x) {
		for (int z = 0; z < depth; ++z) {
			//positions.push_back(vec3(-static_cast<float>(width) / 2.0f + x, MULTIPLIER_HEIGHT * hm[(x * size_multiplier) + ((z * size_multiplier) * SIZE_HEIGHTMAP)], static_cast<float>(depth) / 2.0f - z));
			positions[x + z * width] = vec3(scale * x - (scale * width / 2), (scale / 4) * hm[x + z * width], scale * z - (scale * depth / 2));
		}
	}

	//tex_coords.push_back(vec2((x / (width + 1)), (z / (width + 1))) / 10.0f);

	float width_point = ((float)width) / ((float)SIZE_HEIGHTMAP);
	float depth_point = ((float)depth) / ((float)SIZE_HEIGHTMAP);

	for (float x = 0; x < width; x += 1) {
		for (float z = 0; z < depth; z += 1) {
			tex_coords.push_back(vec2(width_point * x * 4.0f, depth_point * z * 4.0f));
		}
	}

	// Iterate through each vertex and add to geometry
	array<int, 4> verts;
	for (unsigned int x = 0; x < width - 1; ++x) {
		for (unsigned int z = 0; z < depth - 1; ++z)
		{
			// Calculate vertex positions
			verts[0] = x + (z * (depth));
			verts[1] = x + 1 + (z * (depth));
			verts[2] = x + ((z + 1) * (depth));
			verts[3] = x + 1 + ((z + 1) * (depth));

			for (auto &v : verts) {
				minimal = glm::min(minimal, positions[v]);
				maximal = glm::max(maximal, positions[v]);
			}

			// Recalculate minimal and maximal

			// Triangle 1
			indices.push_back(verts[2]);

			indices.push_back(verts[3]);

			indices.push_back(verts[0]);

			// Triangle 2
			indices.push_back(verts[3]);

			indices.push_back(verts[1]);

			indices.push_back(verts[0]);

			// Add normals and colours
			for (unsigned int i = 0; i < 3; ++i)
			{
				colours.push_back(vec4(0.7f, 0.7f, 0.7f, 1.0f));
			}
			for (unsigned int i = 0; i < 3; ++i)
			{
				colours.push_back(vec4(0.7f, 0.7f, 0.7f, 1.0f));
			}
		}
	}

	for (int i = 0; i < indices.size() / 3; i++) {
		int idx[3];
		vec3 sides[2];
		vec3 normal;
		idx[0] = indices[i * 3];
		idx[1] = indices[i * 3 + 1];
		idx[2] = indices[i * 3 + 2];

		sides[0] = positions[idx[0]] - positions[idx[2]];
		sides[1] = positions[idx[0]] - positions[idx[1]];
		//input[1] - input[0], input[2] - input[0]
		normal = cross(sides[1], sides[0]);
		normal = normalize(normal);

		normals[idx[0]] = normals[idx[0]] + normal;
		normals[idx[1]] = normals[idx[1]] + normal;
		normals[idx[2]] = normals[idx[2]] + normal;
	}

	binormals.resize(normals.size());
	tangents.resize(normals.size());
	for (int i = 0; i < normals.size(); i++) {
		normals[i] = normalize(normals[i]);
		binormals[i] = normalize(cross(normals[i], vec3(0.0f, 0.0f, 1.0f))); // aligns binormals to x-axis and makes them perpendicular to normals
		tangents[i] = normalize(cross(normals[i], vec3(1.0f, 0.0f, 0.0f)));
	}

	//// Set minimal and maximal values
	geom.set_minimal_point(minimal);
	geom.set_maximal_point(maximal);
	// Add buffers to geometry
	geom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	geom.add_index_buffer(indices);
	geom.add_buffer(normals, BUFFER_INDEXES::NORMAL_BUFFER);
	geom.add_buffer(binormals, BUFFER_INDEXES::BINORMAL_BUFFER);
	geom.add_buffer(tangents, BUFFER_INDEXES::TANGENT_BUFFER);
	//geom.add_buffer(colours, BUFFER_INDEXES::COLOUR_BUFFER);
	geom.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);

	// Generate tangent and binormal data

	return move(geom);
}

void place_building(vec3 &position, vec3 &rotation, mesh &building_mesh, string building_name, string texture_name, string normal_name) {
	objects[building_name] = scene_object();
	objects[building_name].mesh = building_mesh;
	objects[building_name].mesh.get_transform().rotate(rotation);
	objects[building_name].mesh.get_transform().position = position;
	objects[building_name].tex.push_back(&textures[texture_name]);
	objects[building_name].mesh.get_material().set_diffuse(vec4(0.5f, 0.5f, 0.5f, 1.0f));
	objects[building_name].mesh.get_material().set_emissive(vec4(0.0f));
	objects[building_name].mesh.get_material().set_shininess(0.0f);
	objects[building_name].mesh.get_material().set_specular(vec4(0.0f));
	objects[building_name].eff = &effects["standard_textured"];
	objects[building_name].isInitialized = true;

	if (normal_name != "")
		objects[building_name].norm.push_back(&normal_maps[normal_name]);
}

void place_building_light_point(vec3 &position, mesh &building_mesh, string building_name, string texture_name, string normal_name, vec4 &light_colour, float range) {
	objects[building_name] = scene_object();
	objects[building_name].mesh = building_mesh;
	objects[building_name].mesh.get_transform().rotate(vec3(0.0f, maths::to_radians(float(maths::random(0, 359))), 0.0f));
	objects[building_name].mesh.get_transform().position = position;
	objects[building_name].tex.push_back(&textures[texture_name]);
	objects[building_name].mesh.get_material().set_diffuse(vec4(0.0f));
	objects[building_name].mesh.get_material().set_emissive(vec4(1.0f));
	objects[building_name].mesh.get_material().set_shininess(50.0f);
	objects[building_name].mesh.get_material().set_specular(vec4(1.0f));
	objects[building_name].eff = &effects["standard_textured"];
	objects[building_name].isInitialized = true;

	if (normal_name != "")
		objects[building_name].norm.push_back(&normal_maps[normal_name]);

	light_point light = light_point();
	light.set_parent(&objects[building_name]);
	light.set_colour(light_colour);
	light.set_position(vec3(0.0f, 0.0f, 0.0f));
	light.set_range(range);
	light.set_constant_attenuation(0.1f);
	lights_point.push_back(light);

}

float is_area_buildable(vec2 &position, vector<float> &hm, int mapSizeX, int mapSizeZ) {
	float neighbour_points[4];
	neighbour_points[0] = hm[(position.x - 2) + (position.y - 2) * mapSizeZ];
	neighbour_points[1] = hm[(position.x + 2) + (position.y - 2) * mapSizeZ];
neighbour_points[2] = hm[(position.x - 2) + (position.y + 2) * mapSizeZ];
neighbour_points[3] = hm[(position.x + 2) + (position.y + 2) * mapSizeZ];

float max_difference = 0.0f;
float difference;
float neighbour_lowest = hm[position.x + position.y * mapSizeZ];
for (int i = 0; i < 4; i++) {
	difference = abs(hm[position.x + position.y * mapSizeZ] - neighbour_points[i]);
	if (difference > max_difference)
		max_difference = difference;
	if (neighbour_points[i] < neighbour_lowest)
		neighbour_lowest = neighbour_points[i];

}

if (max_difference > 0.01)
return -1.0f;
else if (neighbour_lowest < 14.0)
	return -1.0f;
else
return neighbour_lowest;
}

void generate_town(int building_count, vector<float> &hm, int mapSizeX, int mapSizeZ, float map_scale) {
	float selected_x = 0;
	float selected_z = 0;
	float neighbour_lowest;
	int building_chance;
	string building_name;
	mesh building_mesh;
	vec3 building_position;
	vec3 building_rotation;
	int house_count = 0;
	for (int repeats = 0; repeats < building_count; repeats++) {
		selected_x = maths::random(32, mapSizeX - 33);
		selected_z = maths::random(32, mapSizeZ - 33);

		neighbour_lowest = is_area_buildable(vec2(selected_x, selected_z), hm, mapSizeX, mapSizeZ);
		building_position = vec3(selected_x * map_scale - ((mapSizeX * map_scale) / 2.0f), neighbour_lowest * (map_scale / 4), selected_z * map_scale - ((mapSizeZ * map_scale) / 2.0f));

		// If too close to another village, find another location
		// This is to stop houses merging
		for (int i = 0; i < village_locations.size(); i++) {
			if (abs(length(vec3(village_locations[i].x, 0.0, village_locations[i].y) - vec3(building_position.x, 0.0, building_position.z))) < 150.0) {
				neighbour_lowest = -1.0;
			}
		}

		// If the lowest neighbour is -1.0, the is_area_buildable function was returning a false
		if (neighbour_lowest == -1.0) {
			repeats--;
			continue;
		}

		building_chance = 0.0;
		village_locations.push_back(vec2(building_position.x, building_position.z));
		building_rotation = vec3(0.0, maths::to_radians(float(maths::random(0, 359))), 0.0);
		if (building_chance == 0) {
			building_name = "tower_dwarf_" + to_string(repeats);
			building_mesh = mesh(geometry("tower_dwarf_05.3ds"));
			place_building(building_position, building_rotation, building_mesh, building_name, "ice", "");
			cout << "pos:: " << building_position.x << "," << building_position.y << "," << building_position.z << endl;
			objects[building_name].radius = 24.0f;
			objects["worldspace"].children.push_back(&objects[building_name]);
			
			building_mesh = mesh(geometry("crystal_03.3ds"));
			building_position = vec3(0.0, 57.5, 0.0);

			place_building_light_point(building_position, building_mesh, building_name + "_crystal", "crystal", "rock_01", vec4(1.0f), 100.0f);
			objects[building_name + "_crystal"].radius = 16.0f;
			objects[building_name].children.push_back(&objects[building_name + "_crystal"]);

			vec3 position_child;
			float rotation_child;
			float distance;

			for (float circles = 0; circles < 2; circles += 1.0) {

				distance = 1.0 + (circles + 1) * 4.0;
				for (int i = -1; i < 2; i++) {
					for (int j = -1; j < 2; j++) {
						if (i == 0 && j == 0) continue;

						// The building will have a 3 in 5 chances of spawning
						building_chance = maths::random(0, 3);
						if (building_chance > 2) continue;

						// Check comment below this method for explanation of this formula
						rotation_child = (22.5 * circles) + ((45.0 * -i) * (j + 2) + ((1 - abs(i)) * (90.0 * (j + 1))));
						float neighbour = is_area_buildable(vec2(selected_x + (i * distance), selected_z + (j * distance)), hm, mapSizeX, mapSizeZ);

						if (neighbour == -1.0f)
							continue;

						position_child.x = distance * sin(maths::to_radians(180.0 + rotation_child)) * map_scale;
						position_child.y = (neighbour - neighbour_lowest) * (map_scale / 4);
						position_child.z = distance * cos(maths::to_radians(180.0 + rotation_child)) * map_scale;

						string name = "house_dwarf_" + to_string(house_count);
						building_mesh = mesh(geometry("house_dwarf_08.3ds"));
						place_building(position_child, vec3(0.0, maths::to_radians(rotation_child), 0.0), building_mesh, name, "rock_03", "");
						objects[name].radius = 16.0f;
						if (abs(length(objects[name].mesh.get_transform().position)) + abs(objects[name].radius) > objects[building_name].radius) {
							objects[building_name].radius = abs(length(objects[name].mesh.get_transform().position)) + abs(objects[name].radius);
						}
						
						objects[building_name].children.push_back(&objects[name]);

						// Each building will have a 3 in 5 chances of spawning an extention. This value will also decide on the position of the extention (right, back, left)
						building_chance = maths::random(0, 4);
						if (building_chance < 3) {
							position_child.x = (building_chance == 0) ? 12.0 : (building_chance == 2) ? -12.0 : 0.0;
							position_child.y = 0.0;
							position_child.z = (building_chance == 1) ? -12.0 : 0.0;

							building_mesh = mesh(geometry("house_dwarf_extention_0" + to_string(maths::random(1, 2)) + ".3ds"));

							place_building(position_child, vec3(0.0, maths::to_radians(90.0 * float(building_chance + 1)), 0.0), building_mesh, name + "_extention", "rock_03", "");
							objects[name + "_extention"].radius = 64.0f;

							if (abs(length(objects[name + "_extention"].mesh.get_transform().position)) + abs(objects[name + "_extention"].radius) > objects[name].radius) {
								objects[name].radius = abs(length(objects[name + "_extention"].mesh.get_transform().position)) + abs(objects[name + "_extention"].radius);
							}
							objects[name].children.push_back(&objects[name + "_extention"]);
						}

						house_count++;
					}
				}
			}
		}
	}
}

/* We want a grid of angles like so:
45     0  -45
90   xxx   90
135 +-180 -135

The grid of (j + 2) and (-i) are:
1 1 1	1 0 -1
2 2 2	1 0 -1
3 3 3	1 0 -1

So a grid of ((45 * -i) * (j + 2)) is:
45 0  -45
90 0  -90
135 0 -135
But we're still missing the 180 in the middle of the last row

the grids of (j + 1) and abs(i) are:
0 0 0	1 0 1
1 1 1	1 0 1
2 2 2	1 0 1

The grids of (90 * (j + 1)) and (1 - abs(i)) are:
0   0   0		0 1 0
90  90  90		0 1 0
180 180 180		0 1 0

So the grid of (1 - abs(i)) * (90 * (j + 1))
0   0 0
0  90 0
0 180 0

So together, the two final grids are give the output desired
*/

bool load_content() {
	geometry geom_screen;
	geom_screen.set_type(GL_QUADS);

	vector<vec3> parts{
		vec3(-1, 1, 0),
		vec3(-1, -1, 0),
		vec3(1, -1, 0),
		vec3(1, 1, 0),
	};

	vector<vec2> colours{
		vec2(0.0f, 1.0f),
		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
	};


	geom_screen.add_buffer(parts, BUFFER_INDEXES::POSITION_BUFFER);
	geom_screen.add_buffer(colours, BUFFER_INDEXES::TEXTURE_COORDS_0);
	objects["screen"].mesh.set_geometry(geom_screen);
	objects["screen"].eff = &effects["screen"];

	textures["grass"] = texture("grass.jpg", true, false);
	textures["rock_01"] = texture("rock_01.jpg", true, false);
	textures["rock_02"] = texture("rock_02.jpg", true, false);
	textures["rock_03"] = texture("rock_03.jpg", true, false);
	textures["sand_red"] = texture("sand_red.jpg", true, false);
	textures["snow"] = texture("snow.jpg", true, false);
	textures["ice"] = texture("ice_01.jpg", true, false);
	textures["rain"] = texture("rain.png", true, false);
	textures["lava"] = texture("lava.jpg", true, false);
	textures["crystal"] = texture("crystal_01.jpg", true, false);

	normal_maps["grass"] = texture("grass_normal.jpg", true, false);
	normal_maps["rock_01"] = texture("rock_01_normal.jpg", true, false);
	normal_maps["rock_02"] = texture("rock_02_normal.jpg", true, false);
	normal_maps["rock_03"] = texture("rock_03_normal.jpg", true, false);
	normal_maps["sand_red"] = texture("sand_red_normal.jpg", true, false);
	normal_maps["snow"] = texture("snow_normal.jpg", true, false);
	normal_maps["water_0"] = texture("water_normal_0.jpg", true, false);
	normal_maps["water_1"] = texture("water_normal_1.jpg", true, false);

	objects["terrain"] = scene_object();

	heightmap_land = generate_continents(SIZE_MAP, SIZE_MAP);
	objects["terrain"].mesh.set_geometry(create_terrain(SIZE_MAP, SIZE_MAP, heightmap_land, SIZE_TILE));

	objects["terrain"].mesh.get_material().set_diffuse(vec4(0.5f, 0.5f, 0.5f, 1.0f));
	objects["terrain"].mesh.get_material().set_emissive(vec4(0.0f));
	objects["terrain"].mesh.get_material().set_shininess(5.0f);
	objects["terrain"].mesh.get_material().set_specular(vec4(0.1f, 0.1f, 0.1f, 1.0f));

	objects["terrain"].tex.push_back(&textures["grass"]);
	objects["terrain"].tex.push_back(&textures["rock_01"]);
	objects["terrain"].tex.push_back(&textures["snow"]);
	objects["terrain"].tex.push_back(&textures["sand_red"]);
	objects["terrain"].norm.push_back(&normal_maps["grass"]);
	objects["terrain"].norm.push_back(&normal_maps["rock_01"]);
	objects["terrain"].norm.push_back(&normal_maps["snow"]);
	objects["terrain"].norm.push_back(&normal_maps["sand_red"]);
	objects["terrain"].eff = &effects["terrain"];
	objects["terrain"].isInitialized = true;
	objects["terrain"].radius = 1200.0;

	objects["worldspace"].children.push_back(&objects["terrain"]);
	objects["worldshadow"].children.push_back(&objects["terrain"]);

	// The sea is just a flat plane that gets distorted in it's geometry shader
	// I use TILE_SIZE / 2 because using a size 8 takes too long to load
	mesh sea = mesh(geometry(geometry_builder::create_plane(SIZE_MAP * 2.0f, SIZE_MAP * 2.0f, SIZE_TILE / 2.0f)));

	sea.get_material().set_diffuse(vec4(0.5f, 0.5f, 0.5f, 1.0f));
	sea.get_material().set_emissive(vec4(0.0f));
	sea.get_material().set_shininess(20.0f);
	sea.get_material().set_specular(vec4(0.8f, 0.8f, 0.8f, 1.0f));
	sea.get_transform().translate(vec3(0.0f, 7.0f, 0.0f));

	objects["sea"] = scene_object();
	objects["sea"].mesh = sea;
	objects["sea"].norm.resize(2);
	objects["sea"].norm[0] = &normal_maps["water_0"];
	objects["sea"].norm[1] = &normal_maps["water_1"];
	objects["sea"].eff = &effects["sea"];
	objects["sea"].isInitialized = true;
	objects["sea"].radius = 1200.0;

	objects["map"] = scene_object();
	objects["map"].mesh = objects["terrain"].mesh;
	objects["map"].mesh.get_transform().scale = vec3(0.005f);
	objects["map"].mesh.get_transform().translate(vec3(0.0f, 0.0f, 0.0f));
	objects["map"].mesh.get_material().set_diffuse(vec4(0.5f, 0.5f, 0.5f, 1.0f));
	objects["map"].mesh.get_material().set_emissive(vec4(0.0f));
	objects["map"].mesh.get_material().set_shininess(5.0f);
	objects["map"].mesh.get_material().set_specular(vec4(0.1f, 0.1f, 0.1f, 1.0f));

	objects["map"].tex.resize(2);
	objects["map"].tex[0] = &textures["grass"];
	objects["map"].tex[1] = &textures["rock_01"];
	objects["map"].eff = &effects["map"];
	objects["map"].isInitialized = false;
	objects["map"].radius = 1200.0;

	objects["rain"].eff = &effects["rain"];

	objects["ring_large"] = scene_object();
	objects["ring_large"].mesh = mesh(geometry("circle_thin.obj"));
	objects["ring_large"].mesh.get_transform().position = vec3(0.0f, 150.0f, 0.0f);
	objects["ring_large"].mesh.get_transform().scale = vec3(4.0f);
	objects["ring_large"].mesh.get_material().set_diffuse(vec4(0.5f, 0.5f, 0.5f, 1.0f));
	objects["ring_large"].mesh.get_material().set_emissive(vec4(0.0f));
	objects["ring_large"].mesh.get_material().set_shininess(20.0f);
	objects["ring_large"].mesh.get_material().set_specular(vec4(0.2f, 0.2f, 0.2f, 1.0f));
	objects["ring_large"].tex.push_back(&textures["snow"]);
	objects["ring_large"].tex.push_back(&normal_maps["snow"]);
	objects["ring_large"].eff = &effects["standard_textured"];
	objects["ring_large"].isInitialized = true;

	objects["worldspace"].children.push_back(&objects["ring_large"]);
	objects["worldshadow"].children.push_back(&objects["ring_large"]);

	objects["ring_medium"] = scene_object();
	objects["ring_medium"].mesh = objects["ring_large"].mesh;
	objects["ring_medium"].mesh.get_transform().position = vec3(0.0f, 0.0f, 0.0f);
	objects["ring_medium"].mesh.get_transform().scale = vec3(0.95f);
	objects["ring_medium"].tex.push_back(&textures["snow"]);
	objects["ring_medium"].tex.push_back(&normal_maps["snow"]);
	objects["ring_medium"].eff = &effects["standard_textured"];
	objects["ring_medium"].isInitialized = true;

	objects["ring_large"].children.push_back(&objects["ring_medium"]);

	objects["ring_small"] = scene_object();
	objects["ring_small"].mesh = objects["ring_medium"].mesh;
	objects["ring_small"].mesh.get_transform().position = vec3(0.0f, 0.0f, 0.0f);
	objects["ring_small"].mesh.get_transform().scale = vec3(0.95f);
	objects["ring_small"].tex.push_back(&textures["snow"]);
	objects["ring_small"].tex.push_back(&normal_maps["snow"]);
	objects["ring_small"].eff = &effects["standard_textured"];
	objects["ring_small"].isInitialized = true;

	objects["ring_medium"].mesh.get_transform().rotate(vec3(pi<float>() / 2, 0.0f, 0.0f));
	objects["ring_small"].mesh.get_transform().rotate(vec3(0.0f, pi<float>() / 2, 0.0f));

	objects["ring_medium"].children.push_back(&objects["ring_small"]);

	objects["sphere_large"] = scene_object();
	objects["sphere_large"].mesh = mesh(geometry_builder::create_sphere(20, 20));
	objects["sphere_large"].mesh.get_transform().position = vec3(0.0f, 0.0f, 0.0f);
	objects["sphere_large"].mesh.get_transform().scale = vec3(1.0f);
	objects["sphere_large"].tex.push_back(&textures["snow"]);
	objects["sphere_large"].tex.push_back(&normal_maps["snow"]);
	objects["sphere_large"].eff = &effects["standard_textured"];
	objects["sphere_large"].isInitialized = true;

	objects["ring_large"].children.push_back(&objects["sphere_large"]);

	objects["sphere_medium"] = scene_object();
	objects["sphere_medium"].mesh = mesh(geometry_builder::create_sphere(20, 20));
	objects["sphere_medium"].mesh.get_transform().position = vec3(0.0f, 5.0f, 0.0f);
	objects["sphere_medium"].mesh.get_transform().scale = vec3(0.5f);
	objects["sphere_medium"].tex.push_back(&textures["snow"]);
	objects["sphere_medium"].tex.push_back(&normal_maps["snow"]);
	objects["sphere_medium"].eff = &effects["standard_textured"];
	objects["sphere_medium"].isInitialized = true;

	objects["sphere_large"].children.push_back(&objects["sphere_medium"]);

	objects["sphere_small"] = scene_object();
	objects["sphere_small"].mesh = mesh(geometry_builder::create_sphere(20, 20));
	objects["sphere_small"].mesh.get_transform().position = vec3(0.0f, 2.0f, 0.0f);
	objects["sphere_small"].mesh.get_transform().scale = vec3(0.75f);
	objects["sphere_small"].tex.push_back(&textures["snow"]);
	objects["sphere_small"].tex.push_back(&normal_maps["snow"]);
	objects["sphere_small"].eff = &effects["standard_textured"];
	objects["sphere_small"].isInitialized = true;

	objects["sphere_medium"].children.push_back(&objects["sphere_small"]);

	objects["player_lamp"] = scene_object();
	objects["player_lamp"].mesh = mesh(geometry_builder::create_sphere(20, 20));
	objects["player_lamp"].mesh.get_transform().position = vec3(0.0f, 1.0f, 0.0f);
	objects["player_lamp"].mesh.get_transform().scale = vec3(0.2f);
	objects["player_lamp"].mesh.get_material().set_diffuse(vec4(1.0f, 0.5f, 0.0f, 1.0f));
	objects["player_lamp"].mesh.get_material().set_specular(vec4(0.0f));
	objects["player_lamp"].mesh.get_material().set_emissive(vec4(0.9f, 0.9f, 0.9f, 1.0f));
	objects["player_lamp"].tex.push_back(&textures["lava"]);
	objects["player_lamp"].tex.push_back(&normal_maps["snow"]);
	objects["player_lamp"].eff = &effects["standard_textured"];
	objects["player_lamp"].isInitialized = true;

	objects["player_darklamp"] = objects["player_lamp"];
	objects["player_darklamp"].mesh.get_material().set_diffuse(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	objects["player_darklamp"].mesh.get_material().set_specular(vec4(0.0f));
	objects["player_darklamp"].mesh.get_material().set_emissive(vec4(0.4f, 0.4f, 0.4f, 1.0f));

	objects["player"] = scene_object();

	objects["player"].children.push_back(&objects["player_lamp"]);
	objects["player"].children.push_back(&objects["player_darklamp"]);

	objects["player"].children.push_back(&objects["map"]);

	objects["worldspace"].children.push_back(&objects["player"]);

	objects["worldspace"].children.push_back(&objects["sea"]);

	objects["weather"].children.push_back(&objects["rain"]);

	generate_town(5, heightmap_land, SIZE_MAP, SIZE_MAP, SIZE_TILE);

	lights_directional.push_back(light_directional());
	lights_directional[0].set_ambient_colour(vec4(0.5, 0.5, 0.5, 1.0f));
	lights_directional[0].set_direction(vec3(0.0f, 0.5f, -1.0f));
	lights_directional[0].set_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));

	lights_directional.push_back(light_directional());
	lights_directional[1].set_ambient_colour(vec4(0.0, 0.0, 0.0, 1.0f));
	lights_directional[1].set_direction(vec3(0.0f, 0.5f, -1.0f));
	lights_directional[1].set_colour(vec4(0.2f, 0.2f, 0.4f, 1.0f));

	lights_point.push_back(light_point());

	lights_point[lights_point.size() - 1] = light_point();
	lights_point[lights_point.size() - 1].set_parent(&objects["player_lamp"]);
	lights_point[lights_point.size() - 1].set_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	lights_point[lights_point.size() - 1].set_position(vec3(0.0f, 0.0f, 0.0f));
	lights_point[lights_point.size() - 1].set_range(20.0f);
	lights_point[lights_point.size() - 1].set_constant_attenuation(0.1f);

	darks_point.push_back(dark_point());

	darks_point[darks_point.size() - 1] = dark_point();
	darks_point[darks_point.size() - 1].set_parent(&objects["player_darklamp"]);
	darks_point[darks_point.size() - 1].set_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	darks_point[darks_point.size() - 1].set_position(vec3(0.0f, 0.0f, 0.0f));
	darks_point[darks_point.size() - 1].set_range(20.0f);
	darks_point[darks_point.size() - 1].set_constant_attenuation(0.1f);

	// Set camera properties
	cam.set_position(vec3(60.0f, 100.0f, 0.0f));
	cam.set_target(vec3(0.0f, 100.0f, 20.0f));
	auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
	cam.set_projection(quarter_pi<float>(), aspect, 0.2f, 1200.0f);

	sphere_cam.set_projection(quarter_pi<float>(), aspect, 0.2f, 1000.0f);

	return true;
}

void set_frustum() {
	float far_dist = 1200.0;
	float near_dist = 0.2;
	float ratio = 1920.0 / 1080.0;
	float fov = quarter_pi<float>();

	float near_h = 2.0 * tan(fov / 2.0) * near_dist;
	float near_w = near_h * ratio;

	float far_h = 2.0 * tan(fov / 2.0) * far_dist;
	float far_w = far_h * ratio;

	vec3 p = cam.get_position();
	vec3 d = normalize(cam.get_target() - cam.get_position());

	vec3 up = normalize(cam.get_up());
	vec3 right = normalize(cross(up, d));

	vec3 far_c = p + d * far_dist;

	vec3 far_tl = far_c + (up * far_h / 2.0f) - (right * far_w / 2.0f);
	vec3 far_tr = far_c + (up * far_h / 2.0f) + (right * far_w / 2.0f);
	vec3 far_bl = far_c - (up * far_h / 2.0f) - (right * far_w / 2.0f);
	vec3 far_br = far_c - (up * far_h / 2.0f) + (right * far_w / 2.0f);

	vec3 near_c = p + d * near_dist;

	vec3 near_tl = near_c + (up * near_h / 2.0f) - (right * near_w / 2.0f);
	vec3 near_tr = near_c + (up * near_h / 2.0f) + (right * near_w / 2.0f);
	vec3 near_bl = near_c - (up * near_h / 2.0f) - (right * near_w / 2.0f);
	vec3 near_br = near_c - (up * near_h / 2.0f) + (right * near_w / 2.0f);

	planes["right"][0] = near_tr;
	planes["right"][1] = normalize(cross(far_tr - near_tr, far_br - near_tr));

	planes["left"][0] = near_bl;
	planes["left"][1] = normalize(cross(far_bl - near_bl, far_tl - near_bl));

	planes["top"][0] = near_tl;
	planes["top"][1] = normalize(cross(far_tl - near_tl, far_tr - near_tl));

	planes["bottom"][0] = near_br;
	planes["bottom"][1] = normalize(cross(far_br - near_br, far_bl - near_br));

	planes["near"][0] = near_tl;
	planes["near"][1] = normalize(cross(near_tr - near_tl, near_br - near_tl));

	planes["far"][0] = far_tr;
	planes["far"][1] = normalize(cross(far_tl - far_tr, far_bl - far_tr));
}

bool PointInFrustum(vec3 &position, float radius) {
	for (auto &element : planes) {
		vec3 point = element.second[0];
		vec3 normal = element.second[1];

		if (dot(point - position, normal) < - radius) {
			return false;
		}
	}
	return true;
}

// Goes through every object and updates its transform and normal matrices with its parents' one
void update_child(scene_object &object, mat4 parent_transform, mat3 parent_normal) {
	mat4 M = parent_transform;
	mat3 N = parent_normal;

	M *= translate(mat4(1.0f), object.mesh.get_transform().position);
	M *= scale(mat4(1.0f), object.mesh.get_transform().scale);
	M *= mat4_cast(object.mesh.get_transform().orientation);

	N *= object.mesh.get_transform().get_normal_matrix();
	
	// Update the object's transform and normal matrices from a cumulation of it's parents and it's own
	object.transform = M;
	object.normal_matrix = N;

	// Updates the object's inFrustum state
	if (object.isInitialized) {
		bool check = false;

		vec4 temp = parent_transform * vec4(object.mesh.get_transform().position, 1.0f);
		vec3 position = vec3(temp.x, temp.y, temp.z);

		if (PointInFrustum(position, object.radius)) {
			check = true;
		}

		object.isInFrustum = check;

	}

	for (scene_object *o : object.children) {
		update_child(*o, M, N);
	}
}

// Updates the camera based on cursor position
void update_cursor(float dT) {
	double mouse_currentX;
	double mouse_currentY;
	glfwGetCursorPos(renderer::get_window(), &mouse_currentX, &mouse_currentY);
	double dX = mouseX - mouse_currentX;
	double dY = mouseY - mouse_currentY;

	cam.rotate(-dX * CAM_TURN_SPEED, dY * CAM_TURN_SPEED);

	mouseX = mouse_currentX;
	mouseY = mouse_currentY;
}

// Resets any toggle when it's key is no longer being pressed
void update_toggles() {
	for (auto option : key_toggles) {
		string option_name = option.first;

		if (!(glfwGetKey(renderer::get_window(), key_bindings[option_name]) || key_toggles[option_name] == false)) {
			key_toggles[option_name] = false;
		}
	}
}

bool update(float delta_time) {
	sun_theta += 0.02f;
	if (sun_theta == 360.0f) {
		sun_theta = 0.0f;
	}

	dT += delta_time;
	if (dT > (4 * pi<float>()))
		dT -= 4 * pi<float>();

	objects["ring_large"].mesh.get_transform().rotate(vec3(0.0f, 0.01f, 0.0f));
	objects["ring_medium"].mesh.get_transform().rotate(vec3(0.02f, 0.0f, 0.0f));
	objects["ring_small"].mesh.get_transform().rotate(vec3(0.0f, 0.04f, 0.0f));

	objects["sphere_medium"].mesh.get_transform().position = vec3(0.0f, 3.0f * sin(maths::to_radians(sun_theta * 20.0f)), 3.0f * cos(maths::to_radians(sun_theta * 20.0f)));
	objects["sphere_medium"].mesh.get_transform().rotate(vec3(0.0f, 0.1f, 0.0f));

	objects["sphere_small"].mesh.get_transform().position = vec3(0.0f, 2.0f * sin(maths::to_radians(sun_theta * 20.0f)), 2.0f * cos(maths::to_radians(sun_theta * 20.0f)));

	lights_directional[0].set_direction(vec3(sun_distance * cos(maths::to_radians(sun_theta)), sun_distance * sin(maths::to_radians(sun_theta)), sun_distance * cos(maths::to_radians(sun_theta))));
	if (sun_theta >= 180.0f) {
		lights_directional[0].set_ambient_colour((sun_theta)* vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}
	
	else if (sun_theta >= 90.0f) {
		lights_directional[0].set_ambient_colour((((90.0f - (sun_theta - 90.0f)) / 90.0f))  * vec4(0.5, 0.5, 0.5, 1.0f));
	}
	
	else {
		lights_directional[0].set_ambient_colour(((sun_theta / 90.0f))  * vec4(0.5, 0.5, 0.5, 1.0f));
	}
	lights_directional[1].set_direction(vec3(sun_distance * cos(maths::to_radians(sun_theta - 180)), sun_distance * sin(maths::to_radians(sun_theta - 180)), sun_distance * cos(maths::to_radians(sun_theta - 180))));

	

	vec3 cam_look = vec3(0.0f);
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_W))
		cam_look += CAM_MOVE_SPEED * (normalize(cam.get_target() - cam.get_position()));
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_S))
		cam_look -= CAM_MOVE_SPEED * (normalize(cam.get_target() - cam.get_position()));
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_D))
		cam_look += CAM_MOVE_SPEED * normalize(cross((normalize(cam.get_target() - cam.get_position())), vec3(0.0f, 1.0f, 0.0f)));
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_A))
		cam_look -= CAM_MOVE_SPEED * normalize(cross((normalize(cam.get_target() - cam.get_position())), vec3(0.0f, 1.0f, 0.0f)));
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_SPACE) || glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT_SHIFT))
		cam_look += CAM_MOVE_SPEED * vec3(0.0f, 1.0f, 0.0);
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_C) || glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT_CONTROL))
		cam_look -= CAM_MOVE_SPEED * vec3(0.0f, 1.0f, 0.0);

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_M) && key_toggles["map_on"] == false) {
		objects["map"].isInitialized = !objects["map"].isInitialized;
		key_toggles["map_on"] = true;
	}
	
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_L) && key_toggles["phong_on"] == false) {
		graphics_toggles["phong_on"] = !graphics_toggles["phong_on"];
		key_toggles["phong_on"] = true;
	}

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_P) && key_toggles["postproc::rain_on"] == false) {
		graphics_toggles["postproc::rain_on"] = !graphics_toggles["postproc::rain_on"];
		key_toggles["postproc::rain_on"] = true;
	}

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_B) && key_toggles["postproc::bloom"] == false) {
		graphics_toggles["postproc::bloom"] = !graphics_toggles["postproc::bloom"];
		key_toggles["postproc::bloom"] = true;
	}

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_G) && key_toggles["postproc::greyscale"] == false) {
		graphics_toggles["postproc::greyscale"] = !graphics_toggles["postproc::greyscale"];
		key_toggles["postproc::greyscale"] = true;
	}

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_H) && key_toggles["postproc::sepia"] == false) {
		graphics_toggles["postproc::sepia"] = !graphics_toggles["postproc::sepia"];
		key_toggles["postproc::sepia"] = true;
	}

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_N) && key_toggles["postproc::motion_blur"] == false) {
		graphics_toggles["postproc::motion_blur"] = !graphics_toggles["postproc::motion_blur"];
		key_toggles["postproc::motion_blur"] = true;
	}

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_V) && key_toggles["postproc::invert"] == false) {
		graphics_toggles["postproc::invert"] = !graphics_toggles["postproc::invert"];
		key_toggles["postproc::invert"] = true;
	}

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_Q) && key_toggles["postproc::cel"] == false) {
		graphics_toggles["postproc::cel"] = !graphics_toggles["postproc::cel"];
		key_toggles["postproc::cel"] = true;
	}

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_E) && key_toggles["postproc::edge"] == false) {
		graphics_toggles["postproc::edge"] = !graphics_toggles["postproc::edge"];
		key_toggles["postproc::edge"] = true;
	}

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_X) && key_toggles["contrast_up"] == false) {
		contrast += 0.1f;
		graphics_toggles["contrast_up"] = !graphics_toggles["contrast_up"];
		key_toggles["contrast_up"] = true;
	}

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_Z) && key_toggles["contrast_down"] == false) {
		contrast -= 0.1f;
		graphics_toggles["contrast_down"] = !graphics_toggles["contrast_down"];
		key_toggles["contrast_down"] = true;
	}

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_I) && key_toggles["sphere_cam_on"] == false) {
		graphics_toggles["sphere_cam_on"] = !graphics_toggles["sphere_cam_on"];
		key_toggles["sphere_cam_on"] = true;
	}

	if (glfwGetKey(renderer::get_window(), key_bindings["weather::rain_on"]) && key_toggles["weather::rain_on"] == false) {
		graphics_toggles["weather::rain_on"] = !graphics_toggles["weather::rain_on"];
		key_toggles["weather::rain_on"] = true;
	}

	if (glfwGetKey(renderer::get_window(), key_bindings["weather::waves_on"]) && key_toggles["weather::waves_on"] == false) {
		graphics_toggles["weather::waves_on"] = !graphics_toggles["weather::waves_on"];
		key_toggles["weather::waves_on"] = true;
	}

	if (glfwGetKey(renderer::get_window(), key_bindings["player_lamp_on"]) && key_toggles["player_lamp_on"] == false) {
		objects["player_lamp"].isInitialized = !objects["player_lamp"].isInitialized;
		objects["player_darklamp"].isInitialized = objects["player_lamp"].isInitialized;
		key_toggles["player_lamp_on"] = true;
	}

	if (glfwGetKey(renderer::get_window(), key_bindings["time::morning"]) && key_toggles["time::morning"] == false) {
		sun_theta = 0.0f;
		key_toggles["time::morning"] = true;
	}

	if (glfwGetKey(renderer::get_window(), key_bindings["time::noon"]) && key_toggles["time::noon"] == false) {
		sun_theta = 90.0f;
		key_toggles["time::noon"] = true;
	}

	if (glfwGetKey(renderer::get_window(), key_bindings["time::evening"]) && key_toggles["time::evening"] == false) {
		sun_theta = 180.0f;
		key_toggles["time::evening"] = true;
	}

	if (glfwGetKey(renderer::get_window(), key_bindings["time::midnight"]) && key_toggles["time::midnight"] == false) {
		sun_theta = 270.0f;
		key_toggles["time::midnight"] = true;
	}

	update_toggles();

	cam.set_position(cam.get_position() + cam_look);
	update_cursor(delta_time);
	// Update the camera
	cam.update(delta_time);
	
	// Updates frustum planes

	// Moves skybox with camera
	skybox.get_transform().position = cam.get_position();
	
	// Sets the player object to the camera position
	objects["player"].mesh.get_transform().position = cam.get_position();
	
	// Uses trigonometry to move the two objects around the player 25 units away
	objects["player_lamp"].mesh.get_transform().position = vec3(25.0f * cos(maths::to_radians(sun_theta * 4.0f)), 0.0f, 25.0f * sin(maths::to_radians(sun_theta * 4.0f)));
	objects["player_darklamp"].mesh.get_transform().position = vec3(25.0f * cos(maths::to_radians(-sun_theta * 4.0f)), 0.0f, 25.0f * sin(maths::to_radians(-sun_theta * 4.0f)));
	
	// Move map so it's always in front of the user, 10 unit vectors away.
	objects["map"].mesh.get_transform().position = 10.0f * (normalize(cam.get_target() - cam.get_position()));

	// Go through all objects. If the object is a crystal, rotate it
	for (auto &object : objects) {
		if (object.first.find("crystal") != string::npos) {
			object.second.mesh.get_transform().rotate(vec3(0.0f, delta_time, 0.0f));
		}
	}

	set_frustum();
	// Update hierarchy's transforms and normal matrices
	update_child(objects["worldspace"], mat4(1.0f), mat3(1.0f)); // x * 1 = x

	sphere_cam.set_position(cam.get_position() + (2.0f * objects["player_lamp"].mesh.get_transform().position) + vec3(0.0f, 5.0f, 0.0f));
	sphere_cam.set_target(cam.get_position() + objects["player_lamp"].mesh.get_transform().position);
	sphere_cam.update(delta_time);


	if (graphics_toggles["weather::rain_on"] == true) {
		int rainCount = 500.0f * delta_time;
		for (int i = 0; i < rainCount; i++) {
			weather_gen::addRain(vec3(((float)maths::random(-200, 200)) / 10.0f, 10.0f + ((float)maths::random(-5, 15) / 2), ((float)maths::random(-200, 200)) / 10.0f), cam.get_position(), rain);
		}
	}

	int i = 0;
	while (true) {
		if (i >= rain.size()) {
			break;
		}
		if ((((rain[i].x + 64) * (MULTIPLIER_WIDTH)) + ((rain[i].z + 64) * (MULTIPLIER_WIDTH)* SIZE_HEIGHTMAP)) > (2047 * 2047) || (((rain[i].x + 64) * (MULTIPLIER_WIDTH)) + ((rain[i].z + 64) * (MULTIPLIER_WIDTH)* SIZE_HEIGHTMAP)) < (0)) {
			if (rain[i].y <= 2.0f) {
				rain.erase(rain.begin() + i, rain.begin() + i + 1);
				continue;
			}
			
			else {
				rain[i].y -= (pi<float>() * delta_time);
				rain[i].x += (wind_dir.x * 25.0f) * delta_time;
				rain[i].z += (wind_dir.z * 25.0f) * delta_time;
				rain[i + 1].y -= (pi<float>() * delta_time);
				rain[i + 1].x = (rain[i].x + (wind_dir.x / 100.0f));
				rain[i + 1].z = (rain[i].z + (wind_dir.z / 100.0f));
			}
		}
		
		else {
			if (rain[i].y <=  7.0f) {
				rain.erase(rain.begin() + i, rain.begin() + i + 1);
				continue;
			}
			else {
				rain[i].y -= (pi<float>() * delta_time);
				rain[i].x += (wind_dir.x * 25.0f) * delta_time;
				rain[i].z += (wind_dir.z * 25.0f) * delta_time;
				rain[i + 1].y -= (pi<float>() * delta_time);
				rain[i + 1].x = (rain[i].x + (wind_dir.x / 100.0f));
				rain[i + 1].z = (rain[i].z + (wind_dir.z / 100.0f));
			}
		}
		i += 2;
	}

	if (rain.size() > 0) {
		geometry geomNew;
		geomNew.set_type(GL_LINES);
		geomNew.add_buffer(rain, BUFFER_INDEXES::POSITION_BUFFER);
		objects["rain"].mesh.set_geometry(geomNew);
		objects["rain"].radius = 3.0f;
		objects["rain"].isInitialized = true;
	}
	
	else {
		objects["rain"].isInitialized = false;
	}
	return true;
}

void bind_to_renderer(const effect &eff, const light_directional &light, const std::string &name) {
	// Check for ambient intensity
	auto idx = eff.get_uniform_location(name + ".ambient_intensity");
	if (idx != -1) {
		glUniform4fv(idx, 1, glm::value_ptr(light.get_ambient_colour()));
	}
	// Check for light colour
	idx = eff.get_uniform_location(name + ".light_colour");
	if (idx != -1) {
		glUniform4fv(idx, 1, glm::value_ptr(light.get_colour()));
	}
	// Check for light direction
	idx = eff.get_uniform_location(name + ".light_dir");
	if (idx != -1) {
		glUniform3fv(idx, 1, glm::value_ptr(light.get_direction()));
	}
	// Check for error
	if (CHECK_GL_ERROR) {
		std::cerr << "ERROR - binding directional light to renderer" << std::endl;
		std::cerr << "OpenGL could not set the uniforms" << std::endl;
		// Throw exception
		throw std::runtime_error("Error using directional light with renderer");
	}
}

void bind_to_renderer(const effect &eff, const light_point &light, const std::string &name) {
	// Check for ambient intensity
	// Check for light colour
	auto idx = eff.get_uniform_location(name + ".colour");

	if (idx != -1) {
		glUniform4fv(idx, 1, glm::value_ptr(light.get_colour()));
	}
	// Check for light direction
	idx = eff.get_uniform_location(name + ".position");
	if (idx != -1) {

		vec3 pos = light.get_position();
		if (light.get_parent() != NULL) {
			pos = vec3((*light.get_parent()).transform * vec4(pos, 1.0f));
		}
		glUniform3fv(idx, 1, glm::value_ptr(pos));
	}
	// Check for light direction
	idx = eff.get_uniform_location(name + ".constant");
	if (idx != -1) {
		glUniform1f(idx, light.get_constant_attenuation());
	}
	// Check for light direction
	idx = eff.get_uniform_location(name + ".linear");
	if (idx != -1) {
		glUniform1f(idx, light.get_linear_attenuation());
	}
	// Check for light direction
	idx = eff.get_uniform_location(name + ".quadratic");
	if (idx != -1) {
		glUniform1f(idx, light.get_quadratic_attenuation());
	}
	// Check for error
	if (CHECK_GL_ERROR) {
		std::cerr << "ERROR - binding directional light to renderer" << std::endl;
		std::cerr << "OpenGL could not set the uniforms" << std::endl;
		// Throw exception
		throw std::runtime_error("Error using directional light with renderer");
	}
}

void bind_to_renderer(const effect &eff, const dark_point &dark, const std::string &name) {
	// Check for ambient intensity
	// Check for light colour
	auto idx = eff.get_uniform_location(name + ".colour");

	if (idx != -1) {
		glUniform4fv(idx, 1, glm::value_ptr(dark.get_colour()));
	}
	// Check for light direction
	idx = eff.get_uniform_location(name + ".position");
	if (idx != -1) {

		vec3 pos = dark.get_position();
		if (dark.get_parent() != NULL) {
			pos = vec3((*dark.get_parent()).transform * vec4(pos, 1.0f));
		}
		glUniform3fv(idx, 1, glm::value_ptr(pos));
	}
	// Check for light direction
	idx = eff.get_uniform_location(name + ".constant");
	if (idx != -1) {
		glUniform1f(idx, dark.get_constant_attenuation());
	}
	// Check for light direction
	idx = eff.get_uniform_location(name + ".linear");
	if (idx != -1) {
		glUniform1f(idx, dark.get_linear_attenuation());
	}
	// Check for light direction
	idx = eff.get_uniform_location(name + ".quadratic");
	if (idx != -1) {
		glUniform1f(idx, dark.get_quadratic_attenuation());
	}
	// Check for error
	if (CHECK_GL_ERROR) {
		std::cerr << "ERROR - binding directional light to renderer" << std::endl;
		std::cerr << "OpenGL could not set the uniforms" << std::endl;
		// Throw exception
		throw std::runtime_error("Error using directional light with renderer");
	}
}

void bind_to_renderer(const effect &eff, const vector<light_directional> &ld) {
	for (int i = 0; i < ld.size(); i++) {
		bind_to_renderer(eff, ld[i], "lights_d[" + to_string(i) + "]");
	}
}

void bind_to_renderer(const effect &eff, const vector<light_point> &lp) {
	for (int i = 0; i < lp.size(); i++) {
		bind_to_renderer(eff, lp[i], "lights_p[" + to_string(i) + "]");
	}
}

void bind_to_renderer(const effect &eff, const vector<dark_point> &dp) {
	for (int i = 0; i < dp.size(); i++) {
		bind_to_renderer(eff, dp[i], "darks_p[" + to_string(i) + "]");
	}
}

// A function to set a specified frame
void set_frame(const frame_buffer &fb, bool clear = true) {
	renderer::set_render_target(fb);
	glClear(GL_DEPTH_BUFFER_BIT);
	if (clear)
		renderer::clear();
}

// A function to set the frame to the default
void set_frame(bool clear = true) {
	renderer::set_render_target();
	glClear(GL_DEPTH_BUFFER_BIT);
	if (clear)
		renderer::clear();
}

// Renders all proper "objects" (children of "worldspace")
void render_child(scene_object &object) {
	if (object.isInitialized) { // If the object is initialised, may check whether or not it's in the view frustum
		if (!object.isInFrustum) { // If this object isn't in view frustum, don't bother loading it or it's children.
			return;
		}
	}
	for (int i = 0; i < object.children.size(); i++) {
		render_child(*object.children[i]);
	}

	if (object.isInitialized) {
		renderer::bind(*object.eff);

		auto M = object.transform; // Get the complete hierarchy-based transform matrix
		auto V = cam.get_view();
		auto P = cam.get_projection();

		if (graphics_toggles["sphere_cam_on"] == true) {
			V = sphere_cam.get_view();
			P = sphere_cam.get_projection();
		}

		auto MVP = P * V * M;

		mat3 N = object.normal_matrix; // Get the complete hierarchy-based normal matrix
		vec3 eye_pos = cam.get_position();
		
		// Bind the three hierarchy-depended matrices
		glUniformMatrix4fv((*object.eff).get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		glUniformMatrix4fv((*object.eff).get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
		glUniformMatrix3fv((*object.eff).get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));

		// Bind all generic parts
		glUniform3fv((*object.eff).get_uniform_location("eye_pos"), 1, value_ptr(eye_pos)); // Most shaders use the eye position
		glUniform1f((*object.eff).get_uniform_location("tile_size"), SIZE_TILE / 2.0f);
		glUniform1f((*object.eff).get_uniform_location("time"), dT);
		glUniform1i((*object.eff).get_uniform_location("lighting_type"), graphics_toggles["phong_on"]); // Tell shader whether to use Gouraud or Phong shading
		glUniform1i((*object.eff).get_uniform_location("create_waves"), graphics_toggles["weather::waves_on"]); // Tell ocean whether or not to create waves
		glUniform1i((*object.eff).get_uniform_location("has_normal_map"), object.norm.size() > 0); // If the object has no normal map, don't attempt to use it
		renderer::bind(object.mesh.get_material(), "mat");
		
		// For terrain rendering, bind village amount and locations
		glUniform1i((*object.eff).get_uniform_location("village_count"), village_locations.size());
		for (int i = 0; i < village_locations.size(); i++) {
			glUniform2fv((*object.eff).get_uniform_location("village_locations[" + to_string(i) + "]"), 1, value_ptr(village_locations[i]));
		}
		
		// Tell the shader how many light/dark sources to read
		// Bind all light and dark sources
		glUniform3fv((*object.eff).get_uniform_location("light_count"), 1, value_ptr(vec3(lights_directional.size(), lights_point.size(), lights_spot.size())));
		glUniform3fv((*object.eff).get_uniform_location("dark_count"), 1, value_ptr(vec3(darks_directional.size(), darks_point.size(), darks_spot.size())));
		bind_to_renderer(*object.eff, lights_directional);
		bind_to_renderer(*object.eff, lights_point);
		bind_to_renderer(*object.eff, darks_point);


		// Load in all textures used by the mesh into an array in the shader
		for (int i = 0; i < object.tex.size(); i++) {
			renderer::bind(*object.tex[i], i);
			glUniform1i((*object.eff).get_uniform_location("tex[" + to_string(i) + "]"), i);
		}

		// Load in all normal maps used by mesh
		for (int i = 0; i < object.norm.size(); i++) {
			renderer::bind(*object.norm[i], i + object.tex.size()); // start binding at the next free number after textures
			glUniform1i((*object.eff).get_uniform_location("normal_map[" + to_string(i) + "]"), i + object.tex.size());
		}

		renderer::render(object.mesh);
	}
}

// Renders the skybox. Brightness depends on time of  day
void render_skybox(const effect &eff, mesh &skybox, const cubemap &tex) {
	renderer::bind(eff);

	// Disable depth dest and depth mask to render seemingly huge skybox
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	auto M = skybox.get_transform().get_transform_matrix();
	auto V = cam.get_view();
	auto P = cam.get_projection();

	if (graphics_toggles["sphere_cam_on"] == true) {
		V = sphere_cam.get_view();
		P = sphere_cam.get_projection();
	}

	auto MVP = P * V * M;
	glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	renderer::bind(tex, 0);
	glUniform1i(eff.get_uniform_location("cubemap"), 0);
	glUniform1f(eff.get_uniform_location("sun_theta"), sun_theta);
	renderer::render(skybox);

	// Re-enable depth test and depth mask for any following renders
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

// Creates a rain-only rendered texture. For use in either distortion, or just blending into main frame
void render_rain(scene_object &rain, const vec3 &eye_pos) {
	if (rain.isInitialized) {
		renderer::bind(*rain.eff);

		auto M = rain.transform;
		auto V = cam.get_view();
		auto P = cam.get_projection();

		if (graphics_toggles["sphere_cam_on"] == true) {
			V = sphere_cam.get_view();
			P = sphere_cam.get_projection();
		}

		auto MVP = P * V * M;

		glUniformMatrix4fv((*rain.eff).get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

		glUniform3fv((*rain.eff).get_uniform_location("eye_pos"), 1, value_ptr(eye_pos));

		renderer::bind(textures["rain"], 0);
		glUniform1i((*rain.eff).get_uniform_location("tex_rain"), 0);

		set_frame(frame_rain);
		renderer::render(rain.mesh);
		set_frame(frame_default, false);
	}
}

// Handles all the post processing toggles before rendering the output to the screen
void render_to_screen() {
	set_frame();
	render_rain(objects["rain"], cam.get_position());
	
	// initialize the post processor's working frame
	post_processor::set_working_frame(frame_default.get_frame());

	post_processor::apply_rain(frame_rain.get_frame(), graphics_toggles["postproc::rain_on"]);


	if (graphics_toggles["postproc::bloom"]) {
		post_processor::apply_bloom(2);
	}

	if (graphics_toggles["postproc::greyscale"]) {
		post_processor::apply_greyscale();

		if (graphics_toggles["postproc::sepia"]) {
			post_processor::apply_sepia();
		}
	}

	if (graphics_toggles["postproc::invert"]) {
		post_processor::apply_invert();
	}

	if (graphics_toggles["postproc::cel"]) {
		post_processor::apply_cel();
	}

	if (graphics_toggles["postproc::edge"]) {
		post_processor::apply_edge();
	}

	post_processor::update();

	if (graphics_toggles["postproc::motion_blur"]) {
		post_processor::apply_motion_blur();
	}

	post_processor::apply_contrast(contrast);

	// Post processing done! Get the resulting frame
	tex_screen = post_processor::get_working_frame();

	// Output resulting frame to screen
	set_frame();
	renderer::bind(*objects["screen"].eff);
	renderer::bind(tex_screen, 0);
	glUniform1i((*objects["screen"].eff).get_uniform_location("frames_screen"), 0);
	renderer::render(objects["screen"].mesh);
}

bool render() {
	// Setting the colour to just blue, because that gives me 2 colours to use in post processing
	// I can't work with blue because it's the background colour
	// We also want to make sure the frame is set to the default frame
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	set_frame(frame_default);

	// First thing we should render is the skybox
	render_skybox(skybox_effect, skybox, skybox_cube);

	// Update world and it's children
	// This has to be called before we render them because the lights need to have
	// updated matrices to use from their parents

	// Render the world (empty placeholder) and all it's children
	render_child(objects["worldspace"]);

	// Render the screen-space quad and any post-processing effects
	render_to_screen();

	return true;
}

void main() {
	// Create application
	app application;
	// Initialize all the frame buffers
	post_processor::initialise(&effects["screen"]);
	frame_default = frame_buffer(1920, 1080);
	frame_rain = frame_buffer(1920, 1080);
	frame_screen = frame_buffer(1920, 1080);
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_update(update);
	application.set_render(render);
	init();
	// Run application
	application.run();
}