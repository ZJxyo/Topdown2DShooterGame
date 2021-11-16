// internal
#include "physics_system.hpp"
#include "world_init.hpp"

// get the world coordiante of collider vertices
std::vector<vec2> get_vertices_location(Entity entity) {
	Motion& motion = registry.motions.get(entity);

	Transform transform;
	transform.translate(motion.position);
	transform.rotate(motion.angle);
	transform.scale(motion.scale);

	std::vector<vec3> collider_vertices = registry.colliders.get(entity).vertices;
	std::vector<vec2> vertices;
	for (vec3& vertex : collider_vertices)
	{
		vec3 transformed_vertex = transform.mat * vertex;
		vertices.push_back(vec2(transformed_vertex.x, transformed_vertex.y));
	}
	return vertices;
}

// Returns x min y min x max y max in vec4
std::vector<float> get_bounding_box(std::vector<vec2>& vertices)
{
	std::vector<float> bb = { vertices[0].x, vertices[0].y, vertices[0].x, vertices[0].y };

	for (int i = 1; i < vertices.size(); i++) {
		bb[0] = min(bb[0], vertices[i].x);
		bb[1] = min(bb[1], vertices[i].y);
		bb[2] = max(bb[2], vertices[i].x);
		bb[3] = max(bb[3], vertices[i].y);
	}

	/*printf("x_min: %f y_min: %f x_max: %f x_max: %f\n", bb[0], bb[0], bb[2], bb[3]);*/

	return bb;
}

bool aabb_collides(std::vector<float> bb1, std::vector<float> bb2)
{
	return bb1[0] < bb2[2] && bb1[2] > bb2[0] && bb1[1] < bb2[3] && bb1[3] > bb2[1];
}

// e1 should be the point and e2 should be the convex polygon
bool point_convex_polygon_collides(vec2 point, std::vector<vec2> vertices) {
	vec2 dir1 = point - vertices[0];
	vec2 dir2 = vertices[1] - vertices[0];

	int index = 1;

	while (index < vertices.size()) {
		dir2 = vertices[index] - vertices[0];
		if (dir1.x * dir2.y - dir2.x * dir1.y > 0) {
			break;
		}
		index++;
	}

	if (index == 1 || index == vertices.size()) {
		return false;
	}

	dir1 = vertices[index - 1] - vertices[index];
	dir2 = point - vertices[index];

	return dir1.x * dir2.y - dir2.x * dir1.y >= 0;
}

void PhysicsSystem::step(float elapsed_ms)
{
	// Move fish based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.

	auto &motion_registry = registry.motions;
	for (uint i = 0; i < motion_registry.size(); i++)
	{
		Motion &motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		float step_seconds = 1.0f * (elapsed_ms / 1000.f);
		motion.position += step_seconds * motion.velocity;
	}

	// all walls' bounding box
	// x min y min x max y max
	std::vector<std::vector<float>> wall_bb;
	for (auto w : registry.walls.entities) {
		std::vector<vec2> location = get_vertices_location(w);
		wall_bb.push_back(get_bounding_box(location));
	}

	// all bullets' bounding box
	std::vector<vec2> bullet_locations;
	std::vector<std::vector<float>> bullet_bb;
	for (Entity e : registry.bullets.entities) {
		std::vector<vec2> location = get_vertices_location(e);
		bullet_locations.push_back(location[0]);
		bullet_bb.push_back(get_bounding_box(location));
	}

	std::vector<std::vector<vec2>> player_locations;
	std::vector<std::vector<float>> player_bb;
	for (Entity p : registry.players.entities) {
		std::vector<vec2> location = get_vertices_location(p);
		player_locations.push_back(location);
		player_bb.push_back(get_bounding_box(location));
	}

	// bullets vs walls
	// in reversed order
	for (int i = registry.bullets.entities.size() - 1; i >= 0; i--) {
		for (std::vector<float> bb : wall_bb) {
			//printf("bullet   x_min: %f y_min: %f x_max: %f x_max: %f\n", bullet_bb[i][0], bullet_bb[i][1], bullet_bb[i][2], bullet_bb[i][3]);
			//printf("wall   x_min: %f y_min: %f x_max: %f x_max: %f\n", bb[0], bb[1], bb[2], bb[3]);
			if (aabb_collides(bullet_bb[i], bb)) {
				registry.remove_all_components_of(registry.bullets.entities[i]);
				bullet_locations[i] = bullet_locations.back();
				bullet_locations.pop_back();
				bullet_bb[i] = bullet_bb.back();
				bullet_bb.pop_back();
				break;
			}
		}
	}

	/*for (int i = registry.bullets.entities.size() - 1; i >= 0; i--) {
		for (int j = registry.players.entities.size() - 1; j >= 0; j--) {
			if (aabb_collides(bullet_bb[i], player_bb[j])) {
				if (point_convex_polygon_collides(bullet_locations[i], player_locations[j])) {
					for (auto callback : callbacks) {
						callback(registry.bullets.entities[i], registry.players.entities[j]);
					}
					registry.remove_all_components_of(registry.bullets.entities[i]);
					bullet_locations[i] = bullet_locations.back();
					bullet_locations.pop_back();
					bullet_bb[i] = bullet_bb.back();
					bullet_bb.pop_back();
					break;
				}
			}
		}
	}*/

	for (int i = registry.players.entities.size() - 1; i >= 0; i--) {
		for (std::vector<float> bb : wall_bb) {
			if (aabb_collides(player_bb[i], bb)) {
			}
		}
	}

	// debugging of bounding boxes
	if (debugging.in_debug_mode)
	{
		uint size_before_adding_new = (uint)registry.motions.entities.size();
		for (uint i = 0; i < size_before_adding_new; i++)
		{
			Motion &motion = registry.motions.components[i];
			Entity entity = registry.motions.entities[i];

			// visualize axises
			Entity line1 = createLine(motion.position, motion.angle, vec2{ 50.f, 3.f});
			Entity line2 = createLine(motion.position, motion.angle, vec2{ 3.f, 50.f });


			if (registry.colliders.has(entity)) {
				std::vector<vec2> vertices = get_vertices_location(entity);

				for (int i = 0; i < vertices.size(); i++) {
					vec2 v1 = vertices[i];
					vec2 v2 = vertices[(i + 1) % vertices.size()];
					vec2 edge = v2 - v1;
					float angle = atan2(edge.y, edge.x);
					vec2 pos = (v2 + v1) / 2.f;
					createLine(pos, angle, vec2{ glm::length(edge), 3.f });
				}
			}
		}
	}
}