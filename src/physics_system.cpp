// internal
#include "physics_system.hpp"
#include "world_init.hpp"

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	if (motion.angle == 0.f) {
		return { abs(motion.scale.x), abs(motion.scale.y) };
	}

	Transform transform;
	transform.rotate(motion.angle);
	vec3 v1 = transform.mat * vec3{ abs(motion.scale.x), abs(motion.scale.y), 1.f };
	vec3 v2 = transform.mat * vec3{ -abs(motion.scale.x), abs(motion.scale.y), 1.f };

	return { max(abs(v1.x), abs(v2.x)), max(abs(v1.y), abs(v2.y)) };
}

bool aabb_collides(const Motion& motion1, const Motion& motion2) {
	const vec2 bb1 = get_bounding_box(motion1) / 2.f;
	const vec2 bb2 = get_bounding_box(motion2) / 2.f;

	return motion1.position.x - bb1.x < motion2.position.x + bb2.x &&
		motion1.position.x + bb1.x > motion2.position.x - bb2.x &&
		motion1.position.y - bb1.y < motion2.position.y + bb2.y &&
		motion1.position.y + bb1.y > motion2.position.y - bb2.y;
}

// get the world coordiante of collider vertices
std::vector<vec2>& get_world_coordinates(const Entity& entity) {
	Motion& motion = registry.motions.get(entity);
	Transform transform;
	transform.translate(motion.position);
	transform.rotate(motion.angle);
	transform.scale(motion.scale);

	Collider& collider = registry.colliders.get(entity);
	std::vector<vec2> vertices;
	for (vec3& vertex : collider.vertices) {
		vec3 transformed_vertex = transform.mat * vertex;
		vertices.push_back(vec2(transformed_vertex.x, transformed_vertex.y));
	}
	return vertices;
}

bool diagonal_collides(const Entity& entity_1, const Entity& entity_2) {
	// get vertex coordinates in world frame
	std::vector<vec2>& vertices_1 = get_world_coordinates(entity_1);
	std::vector<vec2>& vertices_2 = get_world_coordinates(entity_2);

	std::vector<vec2>& v1 = vertices_1;
	std::vector<vec2>& v2 = vertices_2;
	vec2 line_1_vertex_1 = registry.motions.get(entity_1).position;

	for (int i = 0; i < 2; i++)
	{
		for (vec2 line_1_vertex_2 : v1)
		{
			vec2 dir_1 = line_1_vertex_2 - line_1_vertex_1;
			for (int j = 0; j < v2.size(); j++) {
				vec2 line_2_vertex_1 = v2[j];
				vec2 dir_2 = v2[(j + 1) % v2.size()] - line_2_vertex_1;

				vec2 k = line_1_vertex_1 - line_2_vertex_1;
				float h = dir_1.x * dir_1.y - dir_1.x * dir_2.y;
				float t1 = (dir_2.y * k.x + dir_2.x * k.y) / h;
				float t2 = (dir_1.y * k.x + dir_1.x * k.y) / h;
			}
		}
	}

	return false;
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
bool collides(const Entity& entity_1, const Entity& entity_2)
{
	if (aabb_collides(registry.motions.get(entity_1), registry.motions.get(entity_2))) {
		return true;
	}
	return false;
}

void PhysicsSystem::step(float elapsed_ms, float window_width_px, float window_height_px)
{
	// Move fish based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.

	auto &motion_registry = registry.motions;
	for (uint i = 0; i < motion_registry.size(); i++)
	{
		// !!! TODO A1: update motion.position based on step_seconds and motion.velocity
		Motion &motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		float step_seconds = 1.0f * (elapsed_ms / 1000.f);
		if (registry.players.has(entity))
		{
			auto &player = registry.players.get(entity);
			motion.position.x += step_seconds * (player.velocity_left + player.velocity_right);
			motion.position.y += step_seconds * (player.velocity_up + player.velocity_down);
		}
		else
		{
			motion.position += step_seconds * motion.velocity;
		}
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A3: HANDLE PEBBLE UPDATES HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 3
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Check for collisions between all moving entities
	ComponentContainer<Collider>& collider_container = registry.colliders;
	for (uint i = 0; i < collider_container.size(); i++)
	{
		Entity entity_i = collider_container.entities[i];
		for (uint j = i + 1; j < collider_container.size(); j++) // i+1
		{
			assert(i != j);

			Entity& entity_j = collider_container.entities[j];
			if (collides(entity_i, entity_j))
			{
				//CLEAN
				/*if (registry.players.has(entity_i)) {
					printf("Collides: %f\n", registry.motions.get(entity_i).position.x);
				}*/
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
		}
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE SALMON - WALL collisions HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// you may need the following quantities to compute wall positions
	(float)window_width_px; (float)window_height_px;

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: DRAW DEBUG INFO HERE on Salmon mesh collision
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// You will want to use the createLine from world_init.hpp
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	ComponentContainer<Motion>& motion_container = registry.motions;
	// debugging of bounding boxes
	if (debugging.in_debug_mode)
	{
		uint size_before_adding_new = (uint)motion_container.components.size();
		for (uint i = 0; i < size_before_adding_new; i++)
		{
			Motion& motion_i = motion_container.components[i];
			Entity entity_i = motion_container.entities[i];

			// visualize the radius with two axis-aligned lines
			const vec2 bonding_box = get_bounding_box(motion_i);
			float radius = sqrt(dot(bonding_box / 2.f, bonding_box / 2.f));
			vec2 line_scale1 = { motion_i.scale.x / 10, 2 * radius };
			Entity line1 = createLine(motion_i.position, line_scale1);
			vec2 line_scale2 = { 2 * radius, motion_i.scale.x / 10 };
			Entity line2 = createLine(motion_i.position, line_scale2);

			// !!! TODO A2: implement debugging of bounding boxes and mesh
		}
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A3: HANDLE PEBBLE collisions HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 3
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}