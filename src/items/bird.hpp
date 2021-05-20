#pragma once

#include "vcl/vcl.hpp"

struct bird_parameters {
	float const radius_head = 0.25f;
	vcl::vec3 scale_body = { 0.25f, 0.8f, 0.3f };
	float const width_wing = 1.0f;
	float const length_wing = 1.0f;
	float const radius_beak = 0.1f;
	float const height_beak = 0.2f;
};


vcl::hierarchy_mesh_drawable create_bird(float const radius_head, vcl::vec3 const scale_body, float const width_wing, float const length_wing, float const radius_beak, float const height_beak);
vcl::hierarchy_mesh_drawable create_bird(bird_parameters &parameters, float size);
void initialize_bird(vcl::hierarchy_mesh_drawable& bird, float size);