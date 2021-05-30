#include "boat.hpp"
#include <cmath>

using namespace vcl;

vcl::mesh create_boat(float radius, float width, float height, unsigned int N)
{
	mesh boat;
	boat.position.resize(3 * N - 1);
	double alpha = std::acos(1 - width / (2 * radius));
	double beta = 2 * alpha / N;
	float x = 0.0f, y = 0.0f;
	// k = 0
	boat.position[0] = { 0.0f, -radius * std::sin(alpha), height };
	for (int k = 1; k < N; k++) {
		x = radius * std::cos(-alpha + k * beta);
		y = radius * std::sin(-alpha + k * beta);
		boat.position[k] = { width / 2 - radius + x, y, height };
		boat.position[N + k] = { radius - width / 2 - x, y, height };
		boat.position[2 * N + k - 1] = { 0.0f, y, 0.0f };
	}
	// k = N
	boat.position[N] = { 0.0f, radius * std::sin(alpha), height };

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
	boat = vcl::mesh_drawable(create_boat(size * 7.0f, size * 2.0f, size * 1.0f));
	boat.shading.color = { 196.0 / 255, 128.0 / 255, 77.0/255 };
	boat.transform.translate.z = 0.2f;
}

void update_pos_boat(vcl::mesh_drawable& boat, float t)
{
    boat.transform.translate.x += std::sin(t);
}
