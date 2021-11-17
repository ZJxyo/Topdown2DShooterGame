// internal
#include "physics_system.hpp"
#include "world_init.hpp"

// get the world coordiante of collider vertices
std::vector<vec2> get_vertices_world_coordinate(Entity entity) {
	Motion& motion = registry.motions.get(entity);

	Transform transform;
	transform.translate(motion.position);
	transform.rotate(motion.angle);
	transform.scale(motion.scale);

	std::vector<vec3> collider_vertices = registry.polygonColliders.get(entity).vertices;
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

//bool circle_corner_collides(vec2 pos, float radius, std::vector<vec2> vertices) {
//	float min_len = length(vertices[0] - pos);
//	int index = 0;
//
//	for (int i = 1; i < vertices.size(); i++) {
//		float len = length(vertices[i] - pos);
//		if (len < min_len) {
//			min_len = len;
//			index = i;
//		}
//	}
//
//	vec2 axis = vertices[index] - pos;
//
//
//}

void PhysicsSystem::step(float elapsed_ms)
{
	// Move fish based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.

	auto &motion_registry = registry.motions;
	for (uint i = 0; i < motion_registry.size(); i++)
	{
		Motion &motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		motion.position += motion.velocity * elapsed_ms / 1000.f;
	}

	// all walls' bounding box
	std::vector<std::vector<vec2>> wall_vertices;
	std::vector<std::vector<float>> wall_bb;
	for (auto w : registry.walls.entities) {
		std::vector<vec2> vertices = get_vertices_world_coordinate(w);
		wall_vertices.push_back(vertices);
		wall_bb.push_back(get_bounding_box(vertices));
	}

	// all bullets' bounding box
	std::vector<vec2> bullet_vertices;
	std::vector<std::vector<float>> bullet_bb;
	for (Entity e : registry.bullets.entities) {
		vec2 vertex = registry.motions.get(e).position;
		bullet_vertices.push_back(vertex);
		std::vector<float> bb = { vertex.x, vertex.y, vertex.x, vertex.y };
		bullet_bb.push_back(bb);
	}

	// bullets vs walls
	// in reversed order
	for (int i = registry.bullets.entities.size() - 1; i >= 0; i--) {
		// bullet vs boundry
		if (bullet_bb[i][0] < 0 || bullet_bb[i][1] < 0 || bullet_bb[i][2] > 5000 || bullet_bb[i][3] > 5000) {
			registry.remove_all_components_of(registry.bullets.entities[i]);
			bullet_vertices[i] = bullet_vertices.back();
			bullet_vertices.pop_back();
			bullet_bb[i] = bullet_bb.back();
			bullet_bb.pop_back();
			break;
		}

		for (std::vector<float> bb : wall_bb) {
			if (aabb_collides(bullet_bb[i], bb)) {
				registry.remove_all_components_of(registry.bullets.entities[i]);
				bullet_vertices[i] = bullet_vertices.back();
				bullet_vertices.pop_back();
				bullet_bb[i] = bullet_bb.back();
				bullet_bb.pop_back();
				break;
			}
		}
	}

	// bullet vs player/enemies
	for (int i = registry.bullets.entities.size() - 1; i >= 0; i--) {
		for (int j = registry.circleColliders.entities.size() - 1; j >= 0; j--) {
			Entity p = registry.circleColliders.entities[j];
			if (length(bullet_vertices[i] - registry.motions.get(p).position) < registry.circleColliders.components[j].radius) {
				for (auto callback : callbacks) {
					callback(registry.bullets.entities[i], p);
				}
				registry.remove_all_components_of(registry.bullets.entities[i]);
				bullet_vertices[i] = bullet_vertices.back();
				bullet_vertices.pop_back();
				bullet_bb[i] = bullet_bb.back();
				bullet_bb.pop_back();
				break;
			}
		}
	}

	// player/enemies vs walls
	for (int i = registry.circleColliders.entities.size() - 1; i >= 0; i--) {
		Entity p = registry.circleColliders.entities[i];
		Motion& p_motion = registry.motions.get(p);
		vec2& pos = p_motion.position;
		vec2 offset = p_motion.velocity * elapsed_ms / 1000.f;
		float radius = registry.circleColliders.components[i].radius;
		bool restore_x = false;
		bool restore_y = false;
		std::vector<float> bb = { pos.x - radius, pos.y - radius, pos.x + radius, pos.y + radius };
		for (int j = registry.walls.entities.size() - 1; j >= 0; j--) {
			if (restore_x == true && restore_y == true) {
			}
			if (aabb_collides(bb, wall_bb[j])) {
				if (restore_x == false) {
					std::vector<float> bb_x = { bb[0] - offset.x, bb[1], bb[2] - offset.x, bb[3] };
					if (!aabb_collides(bb_x, wall_bb[j])) {
						pos.x -= offset.x;
						bb = bb_x;
						restore_x = true;
						continue;
					}
				}

				if (restore_y == false) {
					std::vector<float> bb_y = { bb[0], bb[1] - offset.y, bb[2], bb[3] - offset.y };
					if (!aabb_collides(bb_y, wall_bb[j])) {
						pos.y -= offset.y;
						bb = bb_y;
						restore_y = true;
						continue;
					}
				}

				pos -= offset;
				break;
			}
		}
		// player/enemies vs boundries
		if (bb[0] < 0) {
			pos.x -= bb[0];
		}

		if (bb[1] < 0) {
			pos.y -= bb[1];
		}

		if (bb[2] > 5000) {
			pos.x -= (bb[2] - 5000);
		}

		if (bb[3] > 5000) {
			pos.y -= (bb[3] - 5000);
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

			if (registry.polygonColliders.has(entity)) {
				std::vector<vec2> vertices = get_vertices_world_coordinate(entity);

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