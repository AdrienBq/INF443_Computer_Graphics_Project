#pragma once

#include "vcl/vcl.hpp"

vcl::mesh create_boat(float radius, float width, float height, unsigned int N = 100);
void initialize_boat(vcl::mesh_drawable& boat, float size);
void initialize_boat(vcl::mesh& boat_mesh, vcl::mesh_drawable& boat);
vcl::vec3 get_boat_pos(vcl::mesh& boat);
void update_pos_boat(vcl::mesh_drawable& boat, float t);
