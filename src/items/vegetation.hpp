#pragma once

#include "vcl/vcl.hpp"

vcl::mesh create_tree_trunk_cylinder(float radius, float height);
vcl::mesh create_palm_leaf(float width = 2.0f, float m = 5.0f, vcl::vec3 v_0 = { 1.0f, 1.0f, 1.0f }, float t_max = 2.0f, unsigned int N = 100, float coef_end = 3.0f);
vcl::hierarchy_mesh_drawable create_palm_tree(float size, int N_leafs=10, float spreading=1.2f);
void initialize_palm_tree(vcl::hierarchy_mesh_drawable& palm_tree, float size);

vcl::mesh create_leaf(float radius, float width, int N);
void rotate_leaf(vcl::mesh &leaf, float alpha, int axis=2);
void translate_leaf(vcl::mesh& leaf, vcl::vec3 p0);
void rule_leaf(std::vector<vcl::mesh>& list_leafs, float r, float w, std::vector<float> list_alphas);
void rule_trunk(std::vector<vcl::mesh>& list_trunks, std::vector<vcl::vec3> list_centers, float r, float h);
void leaf_to_triangles(vcl::mesh &leaf);
vcl::mesh create_fern(float length, float max_width, float radius, float height, int detail_level, int N_leafs = 10);
void initialize_fern(vcl::mesh_drawable& fern, float size);
