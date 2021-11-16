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
std::vector<float> get_bounding_box(Entity entity)
{
	std::vector<vec2> vertices = get_vertices_location(entity);
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

bool point_convex_polygon_collides(const Entity entity_1, const Entity entity_2) {

	const Entity* e1 = &entity_1;
	const Entity* e2 = &entity_2;

	// e1 should be bullet
	if (registry.bullets.has(entity_2))
	{
		e1 = &entity_2;
		e2 = &entity_1;
	}

	std::vector<vec2> v2 = get_vertices_location(*e2);

	vec2 line_1_vertex_1 = registry.motions.get(*e1).position;
	vec2 line_1_vertex_2 = registry.motions.get(*e2).position;
	vec2 dir_1 = line_1_vertex_2 - line_1_vertex_1;

	for (int j = 0; j < v2.size(); j++)
	{
		vec2 line_2_vertex_1 = v2[j];
		vec2 dir_2 = v2[(j + 1) % v2.size()] - line_2_vertex_1;

		vec2 k = line_1_vertex_1 - line_2_vertex_1;
		float det = dir_1.x * dir_2.y - dir_2.x * dir_1.y;

		if (det == 0.f)
		{
			continue;
		}

		float t1 = (dir_2.x * k.y - dir_2.y * k.x) / det;
		float t2 = (dir_1.x * k.y - dir_1.y * k.x) / det;

		// if intersects
		if (t1 > 0.f && t1 < 1.f && t2 > 0.f && t2 < 1.f)
		{
			return false;
		}
	}
	return true;
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

	//ComponentContainer<Collider> &collider_container = registry.colliders;
	//for (uint i = 0; i < collider_container.size(); i++)
	//{
	//	Entity entity_i = collider_container.entities[i];
	//	for (uint j = i + 1; j < collider_container.size(); j++) // i+1
	//	{
	//		assert(i != j);

	//		Entity entity_j = collider_container.entities[j];


	//		// walls shouldn't be colliding
	//		if (registry.walls.has(entity_i) && registry.walls.has(entity_j)) {
	//			continue;
	//		}

	//		if (registry.bullets.has(entity_i) && registry.bullets.has(entity_j)) {
	//			continue;
	//		}

	//		if (collides(entity_i, entity_j))
	//		{

	//			for (auto callback : callbacks) {
	//				callback(entity_i, entity_j);
	//			}
	//		}
	//	}
	//}

	// all walls' bounding box
	// x min y min x max y max
	std::vector<std::vector<float>> wall_bb;
	for (auto w : registry.walls.entities) {
		wall_bb.push_back(get_bounding_box(w));
		/*float* bb = get_bounding_box(w);
		printf("x_min: %f y_min: %f x_max: %f x_max: %f\n", bb[0], bb[0], bb[2], bb[3]);*/
	}

	// all bullets' bounding box
	// in reversed order
	std::vector<std::vector<float>> bullet_bb;
	for (Entity e : registry.bullets.entities) {
		bullet_bb.push_back(get_bounding_box(e));
	}

	// bullets vs walls
	// in reversed order
	for (int i = registry.bullets.entities.size() - 1; i >= 0; i--) {
		for (std::vector<float> bb : wall_bb) {
			//printf("bullet   x_min: %f y_min: %f x_max: %f x_max: %f\n", bullet_bb[i][0], bullet_bb[i][1], bullet_bb[i][2], bullet_bb[i][3]);
			//printf("wall   x_min: %f y_min: %f x_max: %f x_max: %f\n", bb[0], bb[1], bb[2], bb[3]);
			if (aabb_collides(bullet_bb[i], bb)) {
				registry.remove_all_components_of(registry.bullets.entities[i]);
				bullet_bb[i] = bullet_bb.back();
				bullet_bb.pop_back();
				break;
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
			Entity line1 = createLine(motion.position, motion.angle, vec2{ 30.f, 3.f});
			Entity line2 = createLine(motion.position, motion.angle, vec2{ 3.f, 30.f });


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