#pragma once

#include "vcl/vcl.hpp"
#include "boat.hpp"
#include "terrain.hpp"


int NbrSpring = 10;
float const m  = 0.1f;        // particle mass
float const mu = 0.01f;       // damping coefficient
vcl::vec3 const g   = {0,0,-9.81f}; // gravity


vcl::vec3 spring_force(vcl::vec3 const& p_i, vcl::vec3 const& p_j, float L_0, float K);
void initialize_corde(vcl::vec3 pos_bateau, vcl::vec3& pos_poteau, vcl::buffer<vcl::vec3>& particules, vcl::buffer<vcl::vec3>& vitesses, vcl::buffer<float>& L0_array, vcl::buffer<float>& raideurs);
void update_pos_rope(vcl::vec3 pos_bateau, vcl::buffer<vcl::vec3>& particules, vcl::buffer<vcl::vec3>& vitesses, vcl::buffer<float>& L0_array, vcl::buffer<float>& raideurs, vcl::mesh& terrain, float dt);

