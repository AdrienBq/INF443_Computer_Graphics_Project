#pragma once

#include "vcl/vcl.hpp"

//----------------initialisation de la forme du bateau et de sa position de depart-----------------
vcl::mesh create_boat(float radius, float width, float height, unsigned int N = 100);
void initialize_boat(vcl::mesh_drawable& boat, float size);

//----------------update de la position de la barque attachee-----------------
vcl::vec3 get_translation_to_bow(float size);
void update_pos_boat(vcl::mesh_drawable& boat, float t, float tmax);

//----------------update de la position du bateau qui derive sur le fleuve-----------------
void update_boat_drift(vcl::mesh_drawable& boat, float t);
void update_boat_direction(vcl::mesh_drawable &boat, vcl::vec3 position, float theta, bool change_orientation);
