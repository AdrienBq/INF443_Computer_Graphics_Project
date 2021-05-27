#pragma once

#include "vcl/vcl.hpp"

struct perlin_noise_parameters
{
    float persistency = 0.38f;
    float frequency_gain = 2.25f;
    int octave = 6;
    float terrain_height = 0.8f;
};


// Initialize the mesh of the terrain
vcl::mesh initialize_terrain();
vcl::mesh create_terrain();
vcl::vec3 evaluate_terrain2(float u, float v, vcl::mesh& terrain);
vcl::vec3 evaluate_terrain(float u, float v);

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
void update_terrain_water(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters, float t);
void update_terrain_berge_bas(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters);
void update_terrain_berge_haut(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters);
void update_terrain_herbe(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters);
void update_terrain_dune(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters);




