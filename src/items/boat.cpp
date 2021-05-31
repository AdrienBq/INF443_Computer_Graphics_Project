#include "boat.hpp"
#include <cmath>

using namespace vcl;

vcl::mesh create_boat(float radius, float width, float height, unsigned int N)
{
	mesh boat;
	boat.position.resize(3 * N - 1);
	boat.uv.resize(3 * N - 1);
	double alpha = std::acos(1 - width / (2 * radius));
	double beta = 2 * alpha / N;
	float x = 0.0f, y = 0.0f;
	// k = 0
	boat.position[0] = { 0.0f, -radius * std::sin(alpha), height };
	boat.uv[0] = { 0, 0 };
	for (int k = 1; k < N; k++) {
		x = radius * std::cos(-alpha + k * beta);
		y = radius * std::sin(-alpha + k * beta);
		boat.position[k] = { width / 2 - radius + x, y, height };
		boat.position[N + k] = { radius - width / 2 - x, y, height };
		boat.position[2 * N + k - 1] = { 0.0f, y, 0.0f };
		float u = k / N;
		boat.uv[k] = { u, 0 };
		boat.uv[N + k] = { u, 0 };
		boat.uv[2 * N + k - 1] = { u, 1.0f };
	}
	// k = N
	boat.position[N] = { 0.0f, radius * std::sin(alpha), height };
	boat.uv[N] = { 1.0f, 0 };

	for (int i = 1; i < N - 1; i++) {
		boat.connectivity.push_back({ i, i + 1, 2 * N + i });
		boat.connectivity.push_back({ i, 2 * N + i, 2 * N + i - 1 });
		boat.connectivity.push_back({ N + i, N + i + 1, 2 * N + i });
		boat.connectivity.push_back({ N + i, 2 * N + i, 2 * N + i - 1 });
	}
	boat.connectivity.push_back({ 0, 1, 2 * N });
	boat.connectivity.push_back({ 0, N + 1, 2 * N });
	boat.connectivity.push_back({ 3 * N - 2, N - 1, N });
	boat.connectivity.push_back({ 3 * N - 2, 2 * N - 1, N });

	boat.fill_empty_field();
	return boat;
}


void initialize_boat(vcl::mesh_drawable& boat, float size)
{
	boat = vcl::mesh_drawable(create_boat(size * 7.0f, size * 2.0f, size * 1.0f, 50));
	//boat.shading.color = { 196.0 / 255, 128.0 / 255, 77.0/255 };
	boat.transform.translate.z = 0.2f;

	// Load an image from a file
	image_raw const im = image_load_png("pictures/texture_boat_2.png");

	// Send this image to the GPU, and get its identifier texture_image_id
	GLuint const texture_image_id = opengl_texture_to_gpu(im,
		GL_MIRRORED_REPEAT /*GL_CLAMP_TO_EDGE*/ /**GL_TEXTURE_WRAP_S*/,
		GL_MIRRORED_REPEAT /*GL_CLAMP_TO_EDGE*/ /**GL_TEXTURE_WRAP_T*/);

	// Associate the texture_image_id to the image texture used when displaying visual
	boat.texture = texture_image_id;
}

vcl::vec3 get_translation_to_bow(float size)
{
	float R = size * 0.6 * 7.0f, width = size * 2.0f, height = size * 1.0f;
	return { 0, R * std::sin(std::acos(1 - width / R)), height };
}


void update_pos_boat(vcl::mesh_drawable& boat, float t, float tmax)
{
    boat.transform.translate = {4.0f+std::sin(20*pi*(t+ static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 0.01f))) /tmax)/10 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 0.002f)) - 0.001f,-10.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 0.002f)) - 0.001f,0.08f};
}
