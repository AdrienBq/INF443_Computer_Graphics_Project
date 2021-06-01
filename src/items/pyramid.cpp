#include "pyramid.hpp"

using namespace vcl;


// creation de la forme de la pyramide
vcl::mesh create_pyramid(float base, float height)
{
	vcl::mesh pyramid;
	pyramid.position = { {-base / 2, -base / 2, 0.0f},
						 {-base / 2, base / 2, 0.0f},
						 {base / 2, base / 2, 0.0f},
						 {base / 2, -base / 2, 0.0f},
						 {0.0f, 0.0f, height} };

	pyramid.uv = { {0.0f, 1.0f},
					{1.0f, 1.0f},
					{0.0f, 1.0f},
					{1.0f, 1.0f},
					{0.0f, 0.0f} };

	pyramid.connectivity.push_back({ 0,1,2 });
	pyramid.connectivity.push_back({ 2,3,0 });
	pyramid.connectivity.push_back({ 0,1,4 });
	pyramid.connectivity.push_back({ 1,2,4 });
	pyramid.connectivity.push_back({ 2,3,4 });
	pyramid.connectivity.push_back({ 3,0,4 });

	pyramid.fill_empty_field();

	return pyramid;
}

// initialisation du mesh_drawable : taille, texture, position de depart
void initialize_pyramid(vcl::mesh_drawable &pyramid, float size)
{
	pyramid = mesh_drawable(create_pyramid(size * 200.0f, size * 130.0f));
	pyramid.transform.translate.z = 1.0f;
	pyramid.transform.translate.x = -4.0f;
	pyramid.transform.translate.y = -4.0f;
	//pyramid.shading.color = { 0.88f, 0.8f, 0.24f }; // Yellow

	// Load an image from a file
	image_raw const im = image_load_png("pictures/texture_pyramid.png");

	// Send this image to the GPU, and get its identifier texture_image_id
	GLuint const texture_image_id = opengl_texture_to_gpu(im,
		GL_MIRRORED_REPEAT /*GL_CLAMP_TO_EDGE*/ /**GL_TEXTURE_WRAP_S*/,
		GL_MIRRORED_REPEAT /*GL_CLAMP_TO_EDGE*/ /**GL_TEXTURE_WRAP_T*/);

	// Associate the texture_image_id to the image texture used when displaying visual
	pyramid.texture = texture_image_id;
}
