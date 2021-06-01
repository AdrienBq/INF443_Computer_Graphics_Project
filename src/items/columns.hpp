#pragma once

#include "vcl/vcl.hpp"
#include "vegetation.hpp"

// commentaires sur le .cpp

vcl::mesh create_disc(float radius);
vcl::mesh create_column_cyl(float size);
void initialize_column_cyl(vcl::mesh_drawable& column, float size);

vcl::mesh create_obelisque(float base, float height);
void initialize_obelisque(vcl::mesh_drawable &obelisque, float size);
