#include "bird.hpp"
#include "helpers/interpolation.hpp"
#include <cmath>

using namespace vcl;


bird_parameters default_bird;

int idx_last_key_time;


hierarchy_mesh_drawable create_bird(float const radius_head, vec3 const scale_body, float const width_wing, float const length_wing, float const radius_beak, float const height_beak) {

	// The geometry of the head is a sphere
	mesh_drawable head = mesh_drawable(mesh_primitive_sphere(radius_head, { 0,0,0 }, 40, 40));

	// Geometry of the eyes: black spheres
	mesh_drawable eye = mesh_drawable(mesh_primitive_sphere(radius_head / 5, { 0,0,0 }, 20, 20));
	eye.shading.color = { 0,0,0 };

	// Beak
	mesh_drawable beak = mesh_drawable(mesh_primitive_cone(radius_beak, height_beak, { 0,0,0 }, { 0,1,0 }));
	beak.shading.color = { 0,0,0 };

	// Shoulder part and arm are displayed as cylinder
	mesh_drawable shoulder_left = mesh_drawable(mesh_primitive_quadrangle({ 0, -width_wing / 2,0 }, { 0,width_wing / 2,0 }, { -length_wing / 2,width_wing / 2,0 }, { -length_wing / 2,-width_wing / 2,0 }));
	mesh_drawable arm_left = mesh_drawable(mesh_primitive_quadrangle({ 0,-width_wing / 2,0 }, { 0,width_wing / 2,0 }, { -length_wing / 2,width_wing / 4,0 }, { -length_wing / 2,-width_wing / 8,0 }));

	mesh_drawable shoulder_right = mesh_drawable(mesh_primitive_quadrangle({ 0, -width_wing / 2,0 }, { 0,width_wing / 2,0 }, { length_wing / 2,width_wing / 2,0 }, { length_wing / 2,-width_wing / 2,0 }));
	mesh_drawable arm_right = mesh_drawable(mesh_primitive_quadrangle({ 0,-width_wing / 2,0 }, { 0,width_wing / 2,0 }, { length_wing / 2,width_wing / 4,0 }, { length_wing / 2,-width_wing / 8,0 }));

	// An elbow displayed as a sphere
	mesh_drawable elbow = mesh_drawable(mesh_primitive_sphere(radius_head / 1000));

	// Ellipsoid body
	mesh_drawable body = mesh_drawable(mesh_primitive_ellipsoid(scale_body, { 0,0,0 }));


	// Build the hierarchy:
	// ------------------------------------------- //
	
	hierarchy_mesh_drawable hierarchy;

	// Syntax to add element
	//   hierarchy.add(visual_element, element_name, parent_name, (opt)[translation, rotation])

	// The root of the hierarchy is the body
	hierarchy.add(body, "body");

	hierarchy.add(head, "head", "body", { 0.0f, scale_body[1], radius_head / 3 });

	// Eyes positions are set with respect to some ratio of the head
	hierarchy.add(eye, "eye_left", "head", radius_head * vec3(1 / 3.0f, 1 / 2.0f, 1 / 1.5f));
	hierarchy.add(eye, "eye_right", "head", radius_head * vec3(-1 / 3.0f, 1 / 2.0f, 1 / 1.5f));
	hierarchy.add(beak, "beak", "head", radius_head * vec3(0, 0.9f, 0));

	// Set the left part of the body arm: shoulder-elbow-arm
	hierarchy.add(shoulder_left, "shoulder_left", "body", { -scale_body[1] + scale_body[1] / 100,0,0 }); // extremity of the spherical body
	hierarchy.add(elbow, "elbow_left", "shoulder_left", { -length_wing / 2,0,0 });          // place the elbow the extremity of the "shoulder cylinder"
	hierarchy.add(arm_left, "arm_bottom_left", "elbow_left");                        // the arm start at the center of the elbow
	//hierarchy["shoulder_left"].transform.rotate = rotation({ 0,0,1 }, 3.14f / 2);
	hierarchy["shoulder_left"].transform.translate = { scale_body[1] / 500,0,0 };

	// Set the right part of the body arm: similar to the left part with a symmetry in x direction
	hierarchy.add(shoulder_right, "shoulder_right", "body", { scale_body[1] - scale_body[1] / 100,0,0 });
	hierarchy.add(elbow, "elbow_right", "shoulder_right", { length_wing / 2,0,0 });
	hierarchy.add(arm_right, "arm_bottom_right", "elbow_right");
	hierarchy["shoulder_right"].transform.translate = { -scale_body[1] / 500,0,0 };

	return hierarchy;
}



vcl::hierarchy_mesh_drawable create_bird(bird_parameters &parameters, float size)
{
	return create_bird(size * parameters.radius_head, size * parameters.scale_body, size * parameters.width_wing, size * parameters.length_wing, size * parameters.radius_beak, size * parameters.height_beak);
}



void initialize_bird(hierarchy_mesh_drawable& bird, float size)
{
	bird = create_bird(default_bird, size);
	bird["body"].transform.translate = { 3.0f, 0.0f, 5.0f };
	bird.update_local_to_global_coordinates();
}


void initialize_leader_bird(vcl::hierarchy_mesh_drawable& bird, float size, vcl::buffer<vec3> &key_positions, vcl::buffer<float> &key_times)
{
	initialize_bird(bird, size);
	key_positions.push_back(default_bird.key_positions);
	key_times.push_back(default_bird.key_times);
	idx_last_key_time = 1;
	bird["body"].transform.translate = key_positions[1];
	bird["body"].transform.rotate = rotation({ 0,0,1 }, std::asin((key_positions[2][0] - key_positions[1][0]) / norm((key_positions[2] - key_positions[1]))));
	bird.update_local_to_global_coordinates();
}


void update_bird(vcl::hierarchy_mesh_drawable &bird, vcl::vec3 position, float t, float theta, bool change_orientation)
{
	/** *************************************************************  **/
	/** Compute the (animated) transformations applied to the elements **/
	/** *************************************************************  **/

	if (change_orientation) {
		bird["body"].transform.rotate = rotation({ 0,0,1 }, theta);
		bird.update_local_to_global_coordinates();
	}

	bird["body"].transform.translate = position;


	// The head oscillate along the z direction
	//bird["head"].transform.translate = {0,0.01f*(1+std::sin(2*3.14f*t)),0};

	// Rotation of the shoulder-left around the x axis
	bird["shoulder_left"].transform.rotate = rotation({ 0,1,0 }, 0.5f * std::sin(2 * 3.14f * (t - 0.4f) / 1));
	// Rotation of the arm-left around the y axis (delayed with respect to the shoulder)
	bird["arm_bottom_left"].transform.rotate = rotation({ 0,1,0 }, std::sin(2 * 3.14f * (t - 0.6f) / 1));

	// Rotation of the shoulder-right around the y axis
	bird["shoulder_right"].transform.rotate = rotation({ 0,-1,0 }, 0.5f * std::sin(2 * 3.14f * (t - 0.4f) / 1));
	// Rotation of the arm-right around the y axis (delayed with respect to the shoulder)
	bird["arm_bottom_right"].transform.rotate = rotation({ 0,-1,0 }, std::sin(2 * 3.14f * (t - 0.6f) / 1));

	// update the global coordinates
	bird.update_local_to_global_coordinates();
}


void update_leader_bird(vcl::hierarchy_mesh_drawable& bird, float t, float dt, vcl::buffer<vcl::vec3>& key_positions, vcl::buffer<float>& key_times, vcl::buffer<vcl::vec3> &speeds) {
	// INTERPOLATION
	// Compute the interpolated position
	vec3 const p = interpolation(t, key_positions, key_times);
	speeds[speeds.size() - 1] = (p - bird["body"].transform.translate) / dt;
	// Compute the orientation
	int N_t = key_times.size() - 2;
	float theta = 0.0f;
	bool change = false;
	if (idx_last_key_time < N_t && ((int) t) % ((int) key_times[N_t]) > key_times[idx_last_key_time + 1]) {
		int next = idx_last_key_time + 2;
		if (next >= N_t - 2) next = 1;
		theta = -std::acos((key_positions[next][1] - key_positions[idx_last_key_time + 1][1]) / norm(key_positions[next] - key_positions[idx_last_key_time + 1]));
		if ((key_positions[next][0] - key_positions[idx_last_key_time + 1][0]) / norm(key_positions[next] - key_positions[idx_last_key_time + 1]) < 0)
			theta = - theta;
		idx_last_key_time++;
		change = true;
	}
	if (idx_last_key_time >= N_t - 2) {
		idx_last_key_time = 1;
		theta = std::asin((key_positions[2][0] - key_positions[1][0]) / norm((key_positions[2] - key_positions[1])));
	}
	update_bird(bird, p, t, theta, change);
}


void update_follower_birds(vcl::hierarchy_mesh_drawable& leader, vcl::buffer<vcl::vec3> &followers, vcl::buffer<vcl::vec3> &speeds, float t, float dt, float k_attr, float k_rep, float k_frott)
{
	// SIMULATION
	vec3 force, dir;
	float dist;
	const float max_dist = 1.0f;
	int nb = followers.size();
	for (int i = 0; i < nb; i++) {
		force  = { 0, 0, 0 };
		for (int j = 0; j < nb; j++) {
			if (j == i) continue;
			dir = followers[j] - followers[i];
			dist = norm(dir);
			dir /= dist;
			force += /*k_attr * dist * dist * dir / nb*/ - k_rep * dir / (dist * dist) / nb - k_frott * (speeds[i] - speeds[j]) / nb;
		}
		dir = leader["body"].transform.translate - followers[i];
		dist = norm(dir);
		dir /= dist;
		force += 1000000*k_attr * dist * dist * dir - k_rep * dir / (dist * dist) - k_frott * (speeds[i] - speeds[nb]);
		speeds[i] += dt * force;
		followers[i] = followers[i] + dt * speeds[i];
		dir = leader["body"].transform.translate - followers[i];
		dist = norm(dir);
		if (dist > max_dist && dot(speeds[i], speeds[nb]) < 0)
			speeds[i] /= norm(speeds[i]);
	}
}
