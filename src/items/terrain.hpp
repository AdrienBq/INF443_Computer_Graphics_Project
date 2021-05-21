#pragma once

#include "vcl/vcl.hpp"

struct perlin_noise_parameters
{
    float persistency = 0.36f;
    float frequency_gain = 2.25f;
    int octave = 6;
    float terrain_height = 0.5f;
};


// Initialize the mesh of the terrain
vcl::mesh initialize_terrain();

float rive_gauche(float y);
float rive_droite(float y);
float ile(float x);
float dune(float x);

vcl::vec4 heights_dunes(float x);
vcl::vec4 dunes(float x);

bool is_water(float x, float y);
bool is_water_perlin(float x, float y, float noise);
bool is_water1(float x, float y, vcl::buffer<vcl::vec3> *courbes_fleuve);

bool is_berge(float x, float y, float taille_berge);
bool is_dune(float x, float y);
float evaluate_dune(float x, float y, float height);
float evaluate_dune1(float x, float y, float height);

// Recompute the vertices of the terrain everytime a parameter is modified
//  and update the mesh_drawable accordingly
void update_terrain(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters);

vcl::vec3 evaluate_terrain2(float u, float v, vcl::mesh& terrain);
vcl::mesh initialize_terrain();

