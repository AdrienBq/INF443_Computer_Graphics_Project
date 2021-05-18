#include "bird.hpp"

using namespace vcl;


bird_parameters default_bird;

hierarchy_mesh_drawable create_bird(float const radius_head, vec3 const scale_body, float const width_wing, float const length_wing, float const radius_beak, float const height_beak) {

	// The geometry of the head is a sphere
	mesh_drawable head = mesh_drawable(mesh_primitive_sphere(radius_head, { 0,0,0 }, 40, 40));

	// Geometry of the eyes: black spheres
	mesh_drawable eye = mesh_drawable(mesh_primitive_sphere(0.05f, { 0,0,0 }, 20, 20));
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
	mesh_drawable elbow = mesh_drawable(mesh_primitive_sphere(0.001f));

	// Ellipsoid body
	mesh_drawable body = mesh_drawable(mesh_primitive_ellipsoid(scale_body, { 0,0,0 }));


	// Build the hierarchy:
	// ------------------------------------------- //
	
	hierarchy_mesh_drawable hierarchy;

	// Syntax to add element
	//   hierarchy.add(visual_element, element_name, parent_name, (opt)[translation, rotation])

	// The root of the hierarchy is the body
	hierarchy.add(body, "body");

	hierarchy.add(head, "head", "body", { 0.0f, scale_body[1], 0.2f });

	// Eyes positions are set with respect to some ratio of the head
	hierarchy.add(eye, "eye_left", "head", radius_head * vec3(1 / 3.0f, 1 / 2.0f, 1 / 1.5f));
	hierarchy.add(eye, "eye_right", "head", radius_head * vec3(-1 / 3.0f, 1 / 2.0f, 1 / 1.5f));
	hierarchy.add(beak, "beak", "head", radius_head * vec3(0, 0.9f, 0));

	// Set the left part of the body arm: shoulder-elbow-arm
	hierarchy.add(shoulder_left, "shoulder_left", "body", { -scale_body[1] + 0.05f,0,0 }); // extremity of the spherical body
	hierarchy.add(elbow, "elbow_left", "shoulder_left", { -length_wing / 2,0,0 });          // place the elbow the extremity of the "shoulder cylinder"
	hierarchy.add(arm_left, "arm_bottom_left", "elbow_left");                        // the arm start at the center of the elbow
	//hierarchy["shoulder_left"].transform.rotate = rotation({ 0,0,1 }, 3.14f / 2);
	hierarchy["shoulder_left"].transform.translate = { 0.01f,0,0 };

	// Set the right part of the body arm: similar to the left part with a symmetry in x direction
	hierarchy.add(shoulder_right, "shoulder_right", "body", { scale_body[1] - 0.05f,0,0 });
	hierarchy.add(elbow, "elbow_right", "shoulder_right", { length_wing / 2,0,0 });
	hierarchy.add(arm_right, "arm_bottom_right", "elbow_right");
	hierarchy["shoulder_right"].transform.translate = { -0.01f,0,0 };

	return hierarchy;
}



vcl::hierarchy_mesh_drawable create_bird(bird_parameters &parameters, float size)
{
	return create_bird(size * parameters.radius_head, size * parameters.scale_body, size * parameters.width_wing, size * parameters.length_wing, size * parameters.radius_beak, size * parameters.height_beak);
}



void initialize_bird(hierarchy_mesh_drawable& bird, float size)
{
	bird = create_bird(default_bird, size);
	bird["body"].transform.translate.z = 4.0f;
}
