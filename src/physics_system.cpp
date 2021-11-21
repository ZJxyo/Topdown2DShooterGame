// internal
#include "physics_system.hpp"
#include "world_init.hpp"

std::vector<vec3> compute_light_polygon(vec2& pos, std::vector<std::vector<vec2>>& wall_vertices) {
	float half_width = (float)window_width_px / 2.f;
	float half_height = (float)window_height_px / 2.f;
	float radius = half_width + half_height;
	
	// screen corners
	std::vector<vec2> corners = {
		vec2(-half_width, -half_height),
		vec2(half_width, -half_height),
		vec2(half_width, half_height),
		vec2(-half_width, half_height)
	};

	// screen corner angles
	std::vector<float> angles = {
		atan2(corners[0].y, corners[0].x),
		atan2(corners[1].y, corners[1].x),
		atan2(corners[2].y, corners[2].x),
		atan2(corners[3].y, corners[3].x)
	};
	// screen border
	std::vector<vec4> segments = {
		vec4(corners[0], corners[1]),
		vec4(corners[1], corners[2]),
		vec4(corners[2], corners[3]),
		vec4(corners[3], corners[0])
	};

	std::vector<float> bb = get_bounding_box(corners);

	// find all wall edges that might collide with ray
	for (std::vector<vec2> wv : wall_vertices) {
		for (int i = 0; i < wv.size(); i++) {
			vec2 v1 = wv[i] - pos;
			vec2 v2 = wv[(i + 1) % wv.size()] - pos;
			std::vector<vec2> line_vertices = {v1, v2};
			std::vector<float> wbb = get_bounding_box(line_vertices);
			if (aabb_collides(bb, wbb)) {
				segments.push_back(vec4(v1, v2));
				angles.push_back(atan2(clamp(v1.y, -half_height, half_height), clamp(v1.x, -half_width, half_width)));
				angles.push_back(atan2(clamp(v2.y, -half_height, half_height), clamp(v2.x, -half_width, half_width)));
			}
		}
	}

	std::sort(angles.begin(), angles.end());
	angles.erase(std::unique(angles.begin(), angles.end()), angles.end());

	std::vector<vec3> light_polygon;

	vec2 l1v1 = vec2(0, 0);
	for (float angle : angles) {
		for (int j = -1; j < 2; j+=2) {
			// cast 2 rays for each angle
			float new_angle = angle + (float)j * 0.00001;
			vec2 d1 = vec2(cos(new_angle), sin(new_angle)) * radius;
			vec2 l1v2 = l1v1 + d1;

			float smallest_t = 1.f;
			for (vec4 s : segments) {
				vec2 l2v1 = vec2(s.x, s.y);
				vec2 l2v2 = vec2(s.z, s.w);

				vec2 d2 = l2v2 - l2v1;

				float det = d1.x * d2.y - d2.x * d1.y;

				if (det == 0.f)
				{
					continue;
				}

				vec2 k = l1v1 - l2v1;

				float t1 = (d2.x * k.y - d2.y * k.x) / det;
				float t2 = (d1.x * k.y - d1.y * k.x) / det;

				// if intersects
				if (t1 > 0.f && t1 < 1.f && t2 > 0.f && t2 < 1.f)
				{
					smallest_t = min(t1, smallest_t);
				}
			}

			// store the closest colliding position the ray hits
			light_polygon.push_back(vec3(d1.x * smallest_t / half_width, -d1.y * smallest_t / half_height, 0.0f));
		}
	}
	// center point
	light_polygon.push_back(vec3(0.f, 0.f, 0.f));
	return light_polygon;
}

// size: number of vertices of the light polygon
// the last one is the center
std::vector<unsigned int> compute_light_polygon_indices(int size) {
	assert(size > 8);
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < size - 1; i++) {
		indices.push_back(size - 1);
		indices.push_back(i);
		indices.push_back((i + 1) % (unsigned int)(size - 1));
	}
	return indices;
}

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

bool aabb_collides(std::vector<float>& bb1, std::vector<float>& bb2)
{
	return bb1[0] < bb2[2] && bb1[2] > bb2[0] && bb1[1] < bb2[3] && bb1[3] > bb2[1];
}

// e1 should be the point and e2 should be the convex polygon
bool point_convex_polygon_collides(vec2 point, std::vector<vec2>& vertices) {
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

	float time = elapsed_ms / 1000.f;

	auto &motion_registry = registry.motions;
	for (uint i = 0; i < motion_registry.size(); i++)
	{
		Motion &motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		motion.position += motion.velocity * time;
	}

	for (int i = registry.shockwaveSource.size() - 1; i >= 0; i--) {
		registry.shockwaveSource.components[i].time_elapsed += time;
		if (registry.shockwaveSource.components[i].time_elapsed > 1.f) {
			registry.remove_all_components_of(registry.shockwaveSource.entities[i]);
		}
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
				for (auto callback : bullet_hit_callbacks) {
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
		bool restore_xOry = false;
		bool restore_xAndy = false;
		std::vector<float> bb = { pos.x - radius, pos.y - radius, pos.x + radius, pos.y + radius };
		std::vector<float> bb_x = { bb[0] - offset.x, bb[1], bb[2] - offset.x, bb[3] };
		std::vector<float> bb_y = { bb[0], bb[1] - offset.y, bb[2], bb[3] - offset.y };
		for (int j = registry.walls.entities.size() - 1; j >= 0; j--) {
			if (aabb_collides(bb, wall_bb[j])) {
				// is colliding after restoring x
				bool rx = aabb_collides(bb_x, wall_bb[j]);
				// is colliding after restoring y
				bool ry = aabb_collides(bb_y, wall_bb[j]);
				
				if (rx && ry) {
					restore_xAndy = true;
					break;
				}
				else if (rx) {
					restore_y = true;
				}
				else if (ry) {
					restore_x = true;
				}
				else {
					restore_xOry = true;
				}
			}
		}

		bool x_out = bb[0] < 0 || bb[2] > 5000;
		bool y_out = bb[1] < 0 || bb[3] > 5000;

		if (x_out && y_out) {
			restore_xAndy = true;
		}
		else if (x_out) {
			restore_x = true;
		}
		else if (y_out) {
			restore_y = true;
		}

		if (restore_xAndy || (restore_x && restore_y)) {
			pos -= offset;
		}
		else if (restore_x) {
			pos.x -= offset.x;
		}
		else if (restore_y) {
			pos.y -= offset.y;
		}
		else if (restore_xOry) {
			pos.x -= offset.x;
		}
	}

	while (registry.lightSources.size() > 0) {
		registry.remove_all_components_of(registry.lightSources.entities[0]);
	}

	vec2 player_pos = registry.motions.get(registry.players.entities[0]).position;
	std::vector<vec3> light_polygon = compute_light_polygon(player_pos, wall_vertices);
	std::vector<unsigned int> light_polygon_indices = compute_light_polygon_indices(light_polygon.size());
	createLightSource(player_pos, light_polygon, light_polygon_indices);

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