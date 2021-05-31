#pragma once

#include "vcl/vcl.hpp"

vcl::mesh create_boat(float radius, float width, float height, unsigned int N = 100);
void initialize_boat(vcl::mesh_drawable& boat, float size);
void update_pos_boat(vcl::mesh_drawable& boat, float t, float tmax);
void update_boat_drift(vcl::mesh_drawable& boat, float t);
void update_boat_direction(vcl::mesh_drawable &boat, vcl::vec3 position, float theta, bool change_orientation);
