#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:
	void step(float elapsed_ms);

	PhysicsSystem()
	{
	}

	std::vector<std::function<void(Entity entity_1, Entity entity_2)>> bullet_hit_callbacks;
};

std::vector<float> get_bounding_box(std::vector<vec2>& vertices);
bool aabb_collides(std::vector<float>& bb1, std::vector<float>& bb2);
std::vector<vec2> get_vertices_world_coordinate(Entity entity);
std::vector<vec2> compute_light_polygon(float radius, vec2& pos, std::vector<std::vector<vec2>>& wall_vertices);
std::vector<unsigned int> compute_light_polygon_indices(int size);
bool point_convex_polygon_collides(vec2 point, std::vector<vec2>& vertices);