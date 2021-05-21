#include "vcl/vcl.hpp"
#include <iostream>

#include "helpers/scene_helper.hpp"
#include "items/terrain.hpp"
#include "items/pyramid.hpp"
#include "items/vegetation.hpp"
#include "items/bird.hpp"


using namespace vcl;


scene_environment scene;
user_interaction_parameters user;


void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
void window_size_callback(GLFWwindow* window, int width, int height);


void initialize_data();
void display_interface();
void display_frame();

timer_interval timer;

mesh terrain;
mesh_drawable terrain_visual;
perlin_noise_parameters parameters;
mesh_drawable sphere_current;    // sphere used to display the interpolated value
mesh_drawable sphere_keyframe;   // sphere used to display the key positions of berges
buffer<vec3> *courbes_fleuve;

mesh_drawable pyramid;
mesh_drawable palm_tree;
hierarchy_mesh_drawable bird;


int main(int, char* argv[])
{
	std::cout << "Run " << argv[0] << std::endl;

    int const width = 1280, height = 1024;
	GLFWwindow* window = create_window(width, height);
	window_size_callback(window, width, height);
	std::cout << opengl_info_display() << std::endl;;

	imgui_init(window);
	glfwSetCursorPosCallback(window, mouse_move_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	std::cout << "Initialize data ..." << std::endl;
	initialize_data();

	std::cout << "Start animation loop ..." << std::endl;
	user.fps_record.start();
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		scene.light = scene.camera.position();
		user.fps_record.update();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		imgui_create_frame();
		if (user.fps_record.event) {
			std::string const title = "VCL Display - " + str(user.fps_record.fps) + " fps";
			glfwSetWindowTitle(window, title.c_str());
		}

		ImGui::Begin("GUI", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		user.cursor_on_gui = ImGui::IsAnyWindowFocused();

		if (user.gui.display_frame) draw(user.global_frame, scene);

		display_interface();
		display_frame();


		ImGui::End();
		imgui_render_frame(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	imgui_cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}



void initialize_data()
{
	// Basic setups of shaders and camera
	GLuint const shader_mesh = opengl_create_shader_program(opengl_shader_preset("mesh_vertex"), opengl_shader_preset("mesh_fragment"));
	mesh_drawable::default_shader = shader_mesh;
	mesh_drawable::default_texture = opengl_texture_to_gpu(image_raw{ 1,1,image_color_type::rgba,{255,255,255,255} });

	user.global_frame = mesh_drawable(mesh_primitive_frame());
	user.gui.display_frame = false;
	scene.camera.distance_to_center = 2.5f;
	scene.camera.look_at({ -0.5f,2.5f,1 }, { 0,0,0 }, { 0,0,1 });

    // Create the terrain
    terrain = initialize_terrain();
    terrain_visual = mesh_drawable(terrain);
    update_terrain(terrain, terrain_visual, parameters);

    // Initialize drawable structures
    /*sphere_keyframe = mesh_drawable( mesh_primitive_sphere(0.05f) );
    sphere_current  = mesh_drawable( mesh_primitive_sphere(0.06f) );
    sphere_keyframe.shading.color = {0,0,1};
    sphere_current.shading.color  = {1,1,0}; */

	// Pyramid
	initialize_pyramid(pyramid, 8.0f);

	// Palm tree
	initialize_palm_tree(palm_tree, 4.0f);

	// Bird
	initialize_bird(bird, 1.0f);
}



void display_frame()
{
	// Update the current time
    //timer.update();
    //float const t = timer.t;

    ImGui::Checkbox("Frame", &user.gui.display_frame);
    ImGui::Checkbox("Wireframe", &user.gui.display_wireframe);

    bool update = false;
    update |= ImGui::SliderFloat("Persistance", &parameters.persistency, 0.1f, 0.6f);
    update |= ImGui::SliderFloat("Frequency gain", &parameters.frequency_gain, 1.5f, 2.5f);
    update |= ImGui::SliderInt("Octave", &parameters.octave, 1, 8);
    update |= ImGui::SliderFloat("Height", &parameters.terrain_height, 0.1f, 1.5f);

    if(update)// if any slider has been changed - then update the terrain
        update_terrain(terrain, terrain_visual, parameters);

    /*
    for(int i=0; i<3; i++) {
        display_keypositions(sphere_keyframe, courbes_fleuve[i], scene, user.picking);
    }
    */
	
    draw(terrain_visual, scene);
    //draw(pyramid, scene);
    //draw(palm_tree, scene);
    //draw(bird, scene);
}


void display_interface()
{
	ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);
	ImGui::SliderFloat("Time scale", &timer.scale, 0.0f, 2.0f);
	ImGui::Checkbox("Frame", &user.gui.display_frame);
	ImGui::Checkbox("Surface", &user.gui.display_surface);
	ImGui::Checkbox("Wireframe", &user.gui.display_wireframe);
}


void window_size_callback(GLFWwindow*, int width, int height)
{
	glViewport(0, 0, width, height);
	float const aspect = width / static_cast<float>(height);
	float const fov = 50.0f * pi / 180.0f;
	float const z_min = 0.1f;
	float const z_max = 100.0f;
	scene.projection = projection_perspective(fov, aspect, z_min, z_max);
}

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	vec2 const  p1 = glfw_get_mouse_cursor(window, xpos, ypos);
	vec2 const& p0 = user.mouse_prev;
	glfw_state state = glfw_current_state(window);

	auto& camera = scene.camera;
	if (!user.cursor_on_gui) {
		if (state.mouse_click_left && !state.key_ctrl)
			scene.camera.manipulator_rotate_trackball(p0, p1);
		if (state.mouse_click_left && state.key_ctrl)
			camera.manipulator_translate_in_plane(p1 - p0);
		if (state.mouse_click_right)
			camera.manipulator_scale_distance_to_center((p1 - p0).y);
	}

	user.mouse_prev = p1;
}

