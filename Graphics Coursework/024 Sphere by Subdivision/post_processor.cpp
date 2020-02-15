/*
This class takes care of (almost) all the post-processing in the program.
You can't safely work on a frame while using its own texture, so there are
two main frames the program is flipping between - working_frames[2]

The class also has a temporary frame (temp_frame) it uses
to store texture data if it's needed for more than one operation, so as not to overwrite it.
This is only used once in this implementation, in the Bloom effect.

In addition to those, the class has three spaces for frames (the current one and two previous) for use
in motion blur. The one that's being written to is chanced every time update() is called.

Typical usage:
You start off by assigning a frame you want the program to use as it's initial - kind of like an image you first open in photoshop.
To do this, you would call set_working_frame() and pass the texture of you want edited as a parameter.
Next, you would tell the class to apply any effect in any order you like by simply calling the process name
(example: post_processor::apply_motion_blur()).
This would flip the working_frame, then render the previous working_frame's texture to the one currently used.
Once all the effects are applied, you would fetch the texture by calling get_working_frame().
NOTE: Motion Blur won't be applied until at three frames have been stored, so post_processor::update()
has to be called at least three times.
*/

#include "post_processor.h"

// Variables were explained in the header file
frame_buffer post_processor::temp_frame;
frame_buffer post_processor::working_frames[2];
frame_buffer post_processor::frame_previous[3];

bool post_processor::render_motion_blur;
int post_processor::frame_current;
int post_processor::motion_frame;

scene_object post_processor::screen;
texture post_processor::tex_temp;

map<string, effect> post_processor::effects;

// initialises all effects
// All the effects here use "screen.vert" as their vertex shader because they're post processing shaders
// i.e. they work on each texture sent into them, and don't need any information other than texture coordinates
// from the vertex shader
void post_processor::load_effects(effect *screen_effect) {
	effects["invert"].add_shader("screen.vert", GL_VERTEX_SHADER);
	effects["invert"].add_shader("invert.frag", GL_FRAGMENT_SHADER);
	effects["invert"].build();

	effects["contrast"].add_shader("screen.vert", GL_VERTEX_SHADER);
	effects["contrast"].add_shader("contrast.frag", GL_FRAGMENT_SHADER);
	effects["contrast"].build();

	effects["greyscale"].add_shader("screen.vert", GL_VERTEX_SHADER);
	effects["greyscale"].add_shader("greyscale.frag", GL_FRAGMENT_SHADER);
	effects["greyscale"].build();

	effects["sepia"].add_shader("screen.vert", GL_VERTEX_SHADER);
	effects["sepia"].add_shader("sepia.frag", GL_FRAGMENT_SHADER);
	effects["sepia"].build();

	effects["cel"].add_shader("screen.vert", GL_VERTEX_SHADER);
	effects["cel"].add_shader("edge_detect.frag", GL_FRAGMENT_SHADER);
	effects["cel"].add_shader("cel.frag", GL_FRAGMENT_SHADER);
	effects["cel"].build();

	effects["edge"].add_shader("screen.vert", GL_VERTEX_SHADER);
	effects["edge"].add_shader("edge_detect.frag", GL_FRAGMENT_SHADER);
	effects["edge"].add_shader("edge.frag", GL_FRAGMENT_SHADER);
	effects["edge"].build();

	effects["bloom"].add_shader("screen.vert", GL_VERTEX_SHADER);
	effects["bloom"].add_shader("bloom.frag", GL_FRAGMENT_SHADER);
	effects["bloom"].build();

	effects["blur_h"].add_shader("screen.vert", GL_VERTEX_SHADER);
	effects["blur_h"].add_shader("blur_h.frag", GL_FRAGMENT_SHADER);
	effects["blur_h"].build();

	effects["blur_v"].add_shader("screen.vert", GL_VERTEX_SHADER);
	effects["blur_v"].add_shader("blur_v.frag", GL_FRAGMENT_SHADER);
	effects["blur_v"].build();

	effects["blend"].add_shader("screen.vert", GL_VERTEX_SHADER);
	effects["blend"].add_shader("blend.frag", GL_FRAGMENT_SHADER);
	effects["blend"].build();

	effects["rain"].add_shader("screen.vert", GL_VERTEX_SHADER);
	effects["rain"].add_shader("postproc_rain.frag", GL_FRAGMENT_SHADER);
	effects["rain"].build();

	// The output-to-screen effect
	screen.eff = screen_effect;
}

// initialises all frame buffers
void post_processor::load_frame_buffers() {
	temp_frame = frame_buffer(1920, 1080);

	working_frames[0] = frame_buffer(1920, 1080);
	working_frames[1] = frame_buffer(1920, 1080);

	frame_previous[0] = frame_buffer(1920, 1080);
	frame_previous[1] = frame_buffer(1920, 1080);
	frame_previous[2] = frame_buffer(1920, 1080);
}

// initialises all used variables
void post_processor::initialise(effect *screen_effect) {
	frame_current = -1;
	render_motion_blur = false;
	motion_frame = -1;

	load_effects(screen_effect);
	load_frame_buffers();
	
	// Initialise a screen-space quad
	vector<vec3> parts{
		vec3(-1, 1, 0),
		vec3(-1, -1, 0),
		vec3(1, -1, 0),
		vec3(1, 1, 0),
	};
	
	vector<vec2> tex_coords{
		vec2(0.0f, 1.0f),
		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
	};

	geometry geom_screen;
	geom_screen.add_buffer(parts, BUFFER_INDEXES::POSITION_BUFFER);
	geom_screen.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
	geom_screen.set_type(GL_QUADS);

	screen.mesh.set_geometry(geom_screen);
}

// A shortcut for renderer::set_render_target(frame_buffer) + glClear
void post_processor::set_frame(const frame_buffer &fb, bool clear){
	renderer::set_render_target(fb);
	glClear(GL_DEPTH_BUFFER_BIT);
	if (clear)
		renderer::clear();
}

// A shortcut for renderer::set_render_target() + glClear
void post_processor::set_frame(bool clear){
	renderer::set_render_target();
	glClear(GL_DEPTH_BUFFER_BIT);
	if (clear)
		renderer::clear();
}

// Returns the currently selected working frame
// If flip is true, it first flips the currently selected frame index,
// and returns that working frame instead
frame_buffer &post_processor::working_frame_current(bool flip){
	if (flip) {
		frame_current++;
		if (frame_current == 2)
			frame_current = 0;
	}
	return working_frames[frame_current];
}

// Returns the last working frame (the one not currently being worked on)
frame_buffer &post_processor::working_frame_last(){
	return working_frames[(frame_current == 0) ? 1 : 0];
}

// Updates any frame-dependent parts of the post_processor. So far, only motion blur uses this
void post_processor::update() {
	motion_frame++; // change the frame that's being saved to. If the frame is out of bounds, set back to 0 (0, 1, 2, 0, 1, 2, 0, (...))
	if (motion_frame == 3) {
		render_motion_blur = true;
		motion_frame = 0;
	}
	
	// Render the last frame and store it as a frame_previous.
	// this way, it won't be overwritten by the time it's used in motion blur
	set_frame(frame_previous[motion_frame]);
	renderer::bind(*screen.eff);
	renderer::bind(working_frame_current().get_frame(), 0);
	glUniform1i((*screen.eff).get_uniform_location("frames_screen"), 0);
	renderer::render(screen.mesh);
}

// Initialise the working frame to a specified texture
void post_processor::set_working_frame(texture tex_screen) {
	set_frame(working_frame_current(true)); // 0
	renderer::bind(*screen.eff);
	renderer::bind(tex_screen, 0);
	glUniform1i((*screen.eff).get_uniform_location("frame_screen"), 0);
	glUniform1f((*screen.eff).get_uniform_location("cutoff"), 0.5f);
	renderer::render(screen.mesh);
}

// Initialise the working frame to a specified texture
void post_processor::set_working_frame(frame_buffer frame_screen) {
	working_frame_current(true) = frame_screen;
}

// Returns the working frame after the all the specified effects were applied to it
texture post_processor::get_working_frame() {
	return working_frame_current().get_frame();
}

// Applied motion blur to the working frame
void post_processor::apply_motion_blur() {
	if (render_motion_blur) { // Don't try to render the empty frames if they're not initialised yet
		tex_temp = frame_previous[(motion_frame == 2) ? 0 : motion_frame + 1].get_frame(); // ensures the oldest motion_frame is selected (The next incrementation)
		set_working_frame(tex_temp); // initialize the current working frame to use this texture (so it can be called later)
		for (int i = 2; i < 3; i++) {
			int k = (motion_frame + i > 2) ? (motion_frame + i) - 3 : motion_frame + i;
			apply_blend(frame_previous[k].get_frame(), working_frame_current().get_frame(), 0.5f, 0.5f); //
		}

		apply_blend(frame_previous[motion_frame].get_frame(), working_frame_current().get_frame(), 0.5f, 0.5f);
	}
}

// Inverts the colours of the working frame
// Yeah, I know. Kinda useless. Looks cool though
void post_processor::apply_invert(){ // bog-standard effect. Initialised just like an output effect
	set_frame(working_frame_current(true)); // Set the working frame to the one not currently being used
	renderer::bind(effects["invert"]);
	renderer::bind(working_frame_last().get_frame(), 0);
	glUniform1i(effects["invert"].get_uniform_location("frames_screen"), 0);
	renderer::render(screen.mesh);
}

// Applies a specified contrast to the working frame
void post_processor::apply_contrast(float contrast_value){
	set_frame(working_frame_current(true)); // Set the working frame to the one not currently being used
	renderer::bind(effects["contrast"]);
	renderer::bind(working_frame_last().get_frame(), 0);
	glUniform1i(effects["contrast"].get_uniform_location("frames_screen"), 0);
	glUniform1f(effects["contrast"].get_uniform_location("intensity"), contrast_value);
	renderer::render(screen.mesh);
}

// Applies cel-shading to the working frame
void post_processor::apply_cel(){
	set_frame(working_frame_current(true)); // Set the working frame to the one not currently being used
	renderer::bind(effects["cel"]);
	renderer::bind(working_frame_last().get_frame(), 0);
	glUniform1i(effects["cel"].get_uniform_location("frames_screen"), 0);
	renderer::render(screen.mesh);
}

// Applies edge detection to the working frame
void post_processor::apply_edge(){
	set_frame(working_frame_current(true)); // Set the working frame to the one not currently being used
	renderer::bind(effects["edge"]);
	renderer::bind(working_frame_last().get_frame(), 0);
	glUniform1i(effects["edge"].get_uniform_location("frames_screen"), 0);
	renderer::render(screen.mesh);
}

// Applies greyscale to the working frame
void post_processor::apply_greyscale(){
	set_frame(working_frame_current(true)); // Set the working frame to the one not currently being used
	renderer::bind(effects["greyscale"]);
	renderer::bind(working_frame_last().get_frame(), 0);
	glUniform1i(effects["greyscale"].get_uniform_location("frames_screen"), 0);
	renderer::render(screen.mesh);
}

// Applies sepia to the working frame
void post_processor::apply_sepia(){
	set_frame(working_frame_current(true)); // Set the working frame to the one not currently being used
	renderer::bind(effects["sepia"]);
	renderer::bind(working_frame_last().get_frame(), 0);
	glUniform1i(effects["sepia"].get_uniform_location("frames_screen"), 0);
	renderer::render(screen.mesh);
}

// Blends two specified textures with specified blend intensities, saves output to the working frame
// I was debating whether or not to ask for a specific frame_1, since everything else just fetches from the working_frame
// I chose to do it anyway for clarity when reading the code
void post_processor::apply_blend(texture frame_1, texture frame_2, float blend_factor_1, float blend_factor_2){
	set_frame(working_frame_current(true)); // Set the working frame to the one not currently being used

	renderer::bind(effects["blend"]);
	renderer::bind(frame_1, 0);
	renderer::bind(frame_2, 1);
	glUniform1i(effects["blend"].get_uniform_location("frames[0]"), 0);
	glUniform1i(effects["blend"].get_uniform_location("frames[1]"), 1);
	glUniform1f(effects["blend"].get_uniform_location("blend_factor[0]"), blend_factor_1);
	glUniform1f(effects["blend"].get_uniform_location("blend_factor[1]"), blend_factor_2);

	renderer::render(screen.mesh);
}

// Applies bloom to the working frame
void post_processor::apply_bloom(int intensity){
	// create temporary placeholder for the current frame.
	// If I just used a texture, it would be overwritten and I wouldn't be able to use it as in the blend on the final step
	set_frame(temp_frame); // 0
	renderer::bind(*screen.eff);
	renderer::bind(working_frame_current().get_frame(), 0); // Sets frame texture to that of the last rendered frame (since there was no flip, it's the one currently selected)
	glUniform1i((*screen.eff).get_uniform_location("frame_screen"), 0);
	glUniform1f((*screen.eff).get_uniform_location("cutoff"), 0.5f);
	renderer::render(screen.mesh);

	// Gets the brightest points in the image, based on the specified cutoff
	set_frame(working_frame_current(true)); // Set the working frame to the one not currently being used
	renderer::bind(effects["bloom"]);
	renderer::bind(temp_frame.get_frame(), 0);
	glUniform1i(effects["bloom"].get_uniform_location("frame_screen"), 0);
	glUniform1f(effects["bloom"].get_uniform_location("cutoff"), 0.5f);
	renderer::render(screen.mesh);
	
	// Amount of blur passes = intensity
	for (int i = 0; i < intensity; i++) {
		set_frame(working_frame_current(true));
		renderer::bind(effects["blur_h"]); // Blurs once horizontally, then once vertically
		renderer::bind(working_frame_last().get_frame(), 0); // The reason why I went with working_fraes. On the first pass, this is the output of "bloom". On every subsequent, it's the output off "blur_v"
		glUniform1i(effects["blur_h"].get_uniform_location("frame_screen"), 0);
		renderer::render(screen.mesh);

		set_frame(working_frame_current(true));
		renderer::bind(effects["blur_v"]);
		renderer::bind(working_frame_last().get_frame(), 0);
		glUniform1i(effects["blur_v"].get_uniform_location("frame_screen"), 0);
		renderer::render(screen.mesh);
	}
	
	// Blend the original saved frame and the output of the filter/blur together
	// Since we want a pretty intense bloom, set the intensity of the screen AND bloom to 100% when blending
	apply_blend(temp_frame.get_frame(), working_frame_current().get_frame(), 1.0f, 1.0f);
}

// Applies rain effects to the working frame
void post_processor::apply_rain(texture tex_rain, bool distort){
	if (distort) { // If rain distortion is active, applied see-through, distorting rain to the scene
		set_frame(working_frame_current(true)); // Set the working frame to the one not currently being used
		renderer::bind(effects["rain"]);
		renderer::bind(working_frame_last().get_frame(), 0); // Sets frame texture to that of the last rendered frame
		renderer::bind(tex_rain, 1);

		glUniform1i(effects["rain"].get_uniform_location("tex_world"), 0);
		glUniform1i(effects["rain"].get_uniform_location("tex_rain"), 1);
		glUniform4fv(effects["rain"].get_uniform_location("colour_rain"), 1, value_ptr(vec4(1.0f, 0.7f, 0.7f, 0.0f)));
		renderer::render(screen.mesh);
	}
	else { // If rain distortion is inactive, simply adds see-through rain to the scene
		apply_blend(working_frame_current().get_frame(), tex_rain, 1.0f, 0.25f);
	}
}