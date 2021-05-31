#pragma once

#include "vcl/vcl.hpp"

vcl::mesh create_boat(float radius, float width, float height, unsigned int N = 100);
void initialize_boat(vcl::mesh_drawable& boat, float size);
vcl::vec3 get_translation_to_bow(float size);
void update_pos_boat(vcl::mesh_drawable& boat, float t, float tmax);
