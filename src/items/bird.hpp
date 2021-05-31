#pragma once

#include "vcl/vcl.hpp"


struct bird_parameters {
	float const radius_head = 0.05f;
	vcl::vec3 scale_body = { 0.05f, 0.15f, 0.03f };
	float const width_wing = 0.15f;
	float const length_wing = 0.20f;
	float const radius_beak = 0.02f;
	float const height_beak = 0.04;
	vcl::buffer<vcl::vec3> key_positions = { {-7.0f,-10.0f,3.0f}, {-8.0f,-7.2f,3.0f}, {-6.6f,-3.4f,3.0f}, {-5.6f,-0.2f,3.0f}, {-5.4f,1.4f,3.0f}, {1.3f,6.2f,3.0f}, {6.5f,8.8f,3.0f}, {8.0f,9.4f,3.0f},
								{8.0f,4.6f,3.0f}, {6.4f,4.0f,3.0f}, {3.2f,2.4f,3.0f}, {2.6f,0.6f,3.0f}, {4.0f,-2.6f,3.0f}, {6.6f,-6.6f,3.0f}, {8.0f,-8.6f,3.0f},
								{1.5f,-10.0f,3.0f}, {-1.42f,-6.6f,3.0f}, {-4.6f,-6.4f,3.0f}, {-7.0f,-10.0f,3.0f}, {-8.0f,-7.2f,3.0f} };
	vcl::buffer<float> key_times = { 0.0f, 2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 18.0f, 20.0f, 22.0f, 24.0f, 26.0f, 28.0f, 30.0f, 32.0f, 34.0f, 36.0f, 38.0f };
};


vcl::hierarchy_mesh_drawable create_bird(float const radius_head, vcl::vec3 const scale_body, float const width_wing, float const length_wing, float const radius_beak, float const height_beak);
vcl::hierarchy_mesh_drawable create_bird(bird_parameters &parameters, float size);
void initialize_bird(vcl::hierarchy_mesh_drawable& bird, float size);
void initialize_leader_bird(vcl::hierarchy_mesh_drawable& bird, float size, vcl::buffer<vcl::vec3> &key_positions, vcl::buffer<float> &key_times);
void update_bird(vcl::hierarchy_mesh_drawable& bird, vcl::vec3 position, float t, float theta, bool change_orientation);
void update_leader_bird(vcl::hierarchy_mesh_drawable& bird, float t, float dt, vcl::buffer<vcl::vec3>& key_positions, vcl::buffer<float>& key_times, vcl::buffer<vcl::vec3>& speeds);
void update_follower_birds(vcl::hierarchy_mesh_drawable& leader, vcl::buffer<vcl::vec3>& followers, vcl::buffer<vcl::vec3>& speeds, float t, float dt, float k_attr, float k_rep, float k_frott);
