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
std::vector<vec2> get_world_coordinates(const Entity entity) {
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
	//printf("size world: %d\n", vertices.size());
	return vertices;
}

// convex polygons collision detection
// entity_1 can be a wall entity or a non wall entity
// entity_2 cannot be a wall entity
bool diagonal_collides(const Entity entity_1, const Entity entity_2) {
	bool resolve_collision = false;
	const Entity* e1 = &entity_1;
	const Entity* e2 = &entity_2;

	// e2 should be wall
	if (registry.walls.has(entity_1)) {
		resolve_collision = true;
		e1 = &entity_2;
		e2 = &entity_1;
	} else if (registry.walls.has(entity_2)) {
		resolve_collision = true;
	}

	// get vertex coordinates in world frame
	std::vector<vec2> vertices_1 = get_world_coordinates(*e1);
	std::vector<vec2> vertices_2 = get_world_coordinates(*e2);

	std::vector<vec2> v1 = vertices_1;
	std::vector<vec2> v2 = vertices_2;

	assert(!(registry.walls.has(*e1) && registry.walls.has(*e2)));

	Motion& e1_motion = registry.motions.get(*e1);
	vec2 line_1_vertex_1 = e1_motion.position;

	for (int i = 0; i < 2; i++)
	{
		for (vec2 line_1_vertex_2 : v1)
		{
			vec2 dir_1 = line_1_vertex_2 - line_1_vertex_1;
			for (int j = 0; j < v2.size(); j++) {
				vec2 line_2_vertex_1 = v2[j];
				vec2 dir_2 = v2[(j + 1) % v2.size()] - line_2_vertex_1;

				vec2 k = line_1_vertex_1 - line_2_vertex_1;
				float det = dir_1.x * dir_2.y - dir_2.x * dir_1.y;

				if (det == 0.f) {
					continue;
				}

				float t1 = (dir_2.x * k.y - dir_2.y * k.x) / det;
				float t2 = (dir_1.x * k.y - dir_1.y * k.x) / det;

				// if intersects
				if (t1 > 0.f && t1 < 1.f && t2 > 0.f && t2 < 1.f) {
					//printf("%f   %f\n", t1, t2);
					if (resolve_collision) {
						vec2 normalized_normal = normalize(vec2{ -dir_2.y, dir_2.x });
						vec2 displacement = dot((1.f - t1) * dir_1, normalized_normal) * normalized_normal;
						if (i == 1) {
							displacement = -displacement;
						}
						e1_motion.position -= displacement;
						for (vec2& v : v1) {
							v -= displacement;
						}
						continue;
					}
					return true;
				}
			}
		}
		v1 = vertices_2;
		v2 = vertices_1;
		line_1_vertex_1 = registry.motions.get(*e2).position;
	}

	return false;
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
bool collides(const Entity entity_1, const Entity entity_2)
{
	if (aabb_collides(registry.motions.get(entity_1), registry.motions.get(entity_2))) {
		/*if (registry.players.has(entity_1) || registry.players.has(entity_2)) {
			printf("Collides: %f\n", registry.motions.get(entity_1).position.x);
		}*/
		return diagonal_collides(entity_1, entity_2);
	}
	return false;
}

void PhysicsSystem::step(float elapsed_ms)
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

			Entity entity_j = collider_container.entities[j];

			// walls shouldn't be colliding
			if (registry.walls.has(entity_i) && registry.walls.has(entity_j)) {
				continue;
			}

			if (collides(entity_i, entity_j))
			{
				for (auto callback : callbacks) {
					callback(entity_i, entity_j);
				}
			}
		}
	}

	ComponentContainer<Motion>& motion_container = registry.motions;
	// debugging of bounding boxes
	if (debugging.in_debug_mode)
	{
		uint size_before_adding_new = (uint)motion_container.components.size();
		for (uint i = 0; i < size_before_adding_new; i++)
		{
			Motion& motion_i = motion_container.components[i];
			Entity entity_i = motion_container.entities[i];

			// visualize axises
			Entity line1 = createLine(motion_i.position, motion_i.angle, vec2{ 30.f, 3.f});
			Entity line2 = createLine(motion_i.position, motion_i.angle, vec2{ 3.f, 30.f });

			if (registry.colliders.has(entity_i)) {
				std::vector<vec2> transformed_verticies = get_world_coordinates(entity_i);

				for (int i = 0; i < transformed_verticies.size(); i++) {
					vec2 vector1 = transformed_verticies[i];
					vec2 vector2 = transformed_verticies[(i + 1) % transformed_verticies.size()];
					vec2 dir = vector2 - vector1;
					float angle = atan2(dir.y, dir.x);
					vec2 pos = (vector2 + vector1) / 2.f;
					createLine(pos, angle, vec2{ glm::length(dir), 3.f });
				}
			}
		}
	}
}