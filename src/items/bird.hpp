#pragma once

#include "vcl/vcl.hpp"

struct bird_parameters {
	float const radius_head = 0.05f;
	vcl::vec3 scale_body = { 0.05f, 0.15f, 0.03f };
	float const width_wing = 0.15f;
	float const length_wing = 0.20f;
	float const radius_beak = 0.02f;
	float const height_beak = 0.04;
	vcl::buffer<vcl::vec3> key_positions = { {0,-0.5,3}, {-1,-0.5,3}, {1,1,3}, {1,2,3}, {2,2,3}, {2,2,3}, {2,0,3}, {1.5,-1,3}, {1.5,-1,3}, {1,-1,3}, {0,-0.5,3}, {-1,-0.5,3} };
	vcl::buffer<float> key_times = { 0.0f, 1.0f, 2.0f, 2.5f, 3.0f, 3.5f, 3.75f, 4.5f, 5.0f, 6.0f, 7.0f, 8.0f };
};


vcl::hierarchy_mesh_drawable create_bird(float const radius_head, vcl::vec3 const scale_body, float const width_wing, float const length_wing, float const radius_beak, float const height_beak);
vcl::hierarchy_mesh_drawable create_bird(bird_parameters &parameters, float size);
void initialize_bird(vcl::hierarchy_mesh_drawable& bird, float size);
void initialize_leader_bird(vcl::hierarchy_mesh_drawable& bird, float size, vcl::buffer<vcl::vec3> &key_positions, vcl::buffer<float> &key_times);
void update_bird(vcl::hierarchy_mesh_drawable& bird, vcl::vec3 position, float t);
void update_leader_bird(vcl::hierarchy_mesh_drawable& bird, float t, float dt, vcl::buffer<vcl::vec3>& key_positions, vcl::buffer<float>& key_times, vcl::buffer<vcl::vec3>& speeds);
void update_follower_birds(vcl::hierarchy_mesh_drawable& leader, vcl::buffer<vcl::vec3>& followers, vcl::buffer<vcl::vec3>& speeds, float t, float dt, float k_attr, float k_rep, float k_frott);
