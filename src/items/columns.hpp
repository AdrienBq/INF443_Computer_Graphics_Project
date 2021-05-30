#pragma once

#include "vcl/vcl.hpp"
#include "vegetation.hpp"

vcl::mesh create_disc(float radius);
vcl::mesh create_column_cyl(float size);
void initialize_column_cyl(vcl::mesh_drawable& column, float size);
