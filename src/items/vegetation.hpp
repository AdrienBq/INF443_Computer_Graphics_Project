#pragma once

#include "vcl/vcl.hpp"

vcl::mesh create_tree_trunk_cylinder(float radius, float height);
vcl::mesh create_palm_leaf(float width = 2.0f, float m = 5.0f, vcl::vec3 v_0 = { 1.0f, 1.0f, 1.0f }, float t_max = 2.0f, unsigned int N = 100, float coef_end = 3.0f);
vcl::mesh create_palm_tree(float size, int N_leafs=10, float spreading=1.2f);
void initialize_palm_tree(vcl::mesh_drawable& palm_tree, float size);
