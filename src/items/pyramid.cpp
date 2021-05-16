#include "pyramid.hpp"

using namespace vcl;


vcl::mesh create_pyramid(float base, float height)
{
	vcl::mesh pyramid;
	pyramid.position = { {-base / 2, -base / 2, 0.0f},
						 {-base / 2, base / 2, 0.0f},
						 {base / 2, base / 2, 0.0f},
						 {base / 2, -base / 2, 0.0f},
						 {0.0f, 0.0f, height} };

	pyramid.connectivity.push_back({ 0,1,2 });
	pyramid.connectivity.push_back({ 2,3,0 });
	pyramid.connectivity.push_back({ 0,1,4 });
	pyramid.connectivity.push_back({ 1,2,4 });
	pyramid.connectivity.push_back({ 2,3,4 });
	pyramid.connectivity.push_back({ 3,0,4 });

	pyramid.fill_empty_field();

	return pyramid;
}


void initialize_pyramid(vcl::mesh_drawable &pyramid, float size)
{
	pyramid = mesh_drawable(create_pyramid(size, 2*size/3));
	pyramid.transform.translate.z = 1.0f;
	pyramid.shading.color = { 0.88f, 0.8f, 0.24f }; // Yellow
}
