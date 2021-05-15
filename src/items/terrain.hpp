#pragma once

#include "vcl/vcl.hpp"

struct perlin_noise_parameters
{
    float persistency = 0.36f;
    float frequency_gain = 2.25f;
    int octave = 6;
    float terrain_height = 1.0f;
};


// Initialize the mesh of the terrain
vcl::mesh initialize_terrain();

bool is_water(float u, float v);

// Recompute the vertices of the terrain everytime a parameter is modified
//  and update the mesh_drawable accordingly
void update_terrain(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters);

vcl::vec3 evaluate_terrain2(float u, float v, vcl::mesh& terrain);
vcl::mesh initialize_terrain();

