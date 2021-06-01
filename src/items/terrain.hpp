#pragma once

#include "vcl/vcl.hpp"

//----------------bruit de perlin utilise pour le terrain-----------------

struct perlin_noise_parameters
{
    float persistency = 0.38f;
    float frequency_gain = 2.25f;
    int octave = 6;
    float terrain_height = 0.4f;
};

perlin_noise_parameters get_noise_params();

//----------------initialisation du terrain et fonctions permettant de retrouver la position d'un point sur le mesh du terrain-----------------

vcl::mesh create_terrain();
vcl::vec3 evaluate_terrain2(float u, float v, vcl::mesh& terrain);
vcl::vec3 evaluate_terrain(float u, float v);
float evaluate_dune(float x, float y, float height);

vcl::vec4 heights_dunes(float x);
vcl::vec4 dunes(float x);

//----------------fonctions de base polynomiales definissant les differentes parties du terrain-----------------

float rive_gauche(float y);
float rive_gauche_d(float y);
float rive_droite(float y);
float rive_droite_d(float y);
float ile(float x);
float ile_d(float x);
float dune(float x);


bool is_water(float x, float y);
bool is_rive_droite(float x, float y);
bool is_berge(float x, float y, float taille_berge);
bool is_dune(float x, float y);

//----------------utilisation des fonctions ci dessus pour actualiser les points du terrain-----------------

void update_terrain(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual1, vcl::mesh_drawable& terrain_visual2, vcl::mesh_drawable& terrain_visual3, vcl::mesh_drawable& terrain_visual4, vcl::mesh_drawable& terrain_visual5, vcl::mesh_drawable& terrain_visual6, perlin_noise_parameters const& parameters, float t, float tmax);

void update_terrain_water(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters, float t, float tmax);
void update_terrain_berge_bas(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters);
void update_terrain_berge_milieu(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters);
void update_terrain_berge_haut(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters);
void update_terrain_herbe(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters);
void update_terrain_rive_droite(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters);
void update_terrain_dune(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters);

GLuint texture(const std::string& filename);

//----------------generation des positions des elements sur le terrain-----------------

std::vector<vcl::vec3> generate_positions_forest(int N, vcl::mesh& terrain);
std::vector<vcl::vec3> generate_positions_pyramids(vcl::mesh& terrain, perlin_noise_parameters const& parameters);
std::vector<vcl::vec3> generate_positions_ferns(vcl::mesh& terrain, perlin_noise_parameters const& parameters);
std::vector<vcl::vec3> generate_positions_columns(vcl::mesh& terrain, perlin_noise_parameters const& parameters);
std::vector<vcl::vec3> generate_positions_columns2(vcl::mesh& terrain, perlin_noise_parameters const& parameters);
std::vector<vcl::vec3> generate_positions_obelisque(vcl::mesh& terrain, perlin_noise_parameters const& parameters);
