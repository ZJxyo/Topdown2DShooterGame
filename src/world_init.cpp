#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include <map>
#include <list>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

using namespace std;
using namespace nlohmann;

Entity createSalmon(RenderSystem *renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;

	motion.velocity = { 0.f, 0.f };
	motion.scale = {150,100};


	// Create and (empty) Salmon component to be able to refer to all turtles
	registry.players.emplace(entity);
	registry.healths.emplace(entity, 100);
	registry.colliders.emplace(entity);
	registry.animates.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::PLAYER,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE});
	return entity;
}

Entity createWall(RenderSystem *renderer, vec2 pos, float angle, vec2 scale)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::RECTANGLE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = angle;
	motion.scale = scale;

	registry.colliders.emplace(entity);
	registry.walls.emplace(entity);

	// Create and (empty) Salmon component to be able to refer to all turtles
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
		 EFFECT_ASSET_ID::PEBBLE,
		 GEOMETRY_BUFFER_ID::RECTANGLE});

	return entity;
}

Entity createFish(RenderSystem *renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {-50, 0};
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({-FISH_BB_WIDTH, FISH_BB_HEIGHT});

	// Create an (empty) Fish component to be able to refer to all fish
	registry.softShells.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::FISH,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}

Entity createTurtle(RenderSystem *renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);
	registry.colliders.emplace(entity);

	// Initialize the motion
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {100.f, 0.f};
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({150, 150});

	// Create and (empty) Turtle component to be able to refer to all turtles
	registry.hardShells.emplace(entity);
	registry.animates.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::PLAYER,
		 EFFECT_ASSET_ID::TURTLE,
		 GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}

Entity createLine(vec2 position, float angle, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::PEBBLE,
		 GEOMETRY_BUFFER_ID::RECTANGLE});

	// Create motion

	Motion& motion = registry.motions.emplace(entity);
	motion.angle = angle;
	motion.velocity = { 0, 0 };

	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);
	return entity;
}

Entity createPebble(vec2 pos, vec2 size)
{
	auto entity = Entity();

	// Setting initial motion values
	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = {0.f, 0.f};
	motion.scale = size;

	// Create and (empty) Salmon component to be able to refer to all turtles
	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
		 EFFECT_ASSET_ID::PEBBLE,
		 GEOMETRY_BUFFER_ID::PEBBLE});

	return entity;
}
struct wall
{
	vec2 position;
	float angle;
	vec2 scale;
};

int SetupMap(RenderSystem *renderer)
{
	string src = PROJECT_SOURCE_DIR;
	src += "src/map/map.json";
	ifstream ifs(src);
	json j;
	ifs >> j;

	for (json w : j["walls"])
	{
		auto entity = Entity();

		// Store a reference to the potentially re-used mesh object
		Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::RECTANGLE);
		registry.meshPtrs.emplace(entity, &mesh);

		// Setting initial motion values
		Motion &motion = registry.motions.emplace(entity);
		int value1 = int(w["position"]["x"]) + 50;
		int value2 = int(w["position"]["y"]) + 50;
		motion.position = vec2(value1, value2);
		motion.angle = w["angle"];
		motion.scale = vec2(w["scale"]["x"], w["scale"]["y"]);

		registry.colliders.emplace(entity);
		registry.walls.emplace(entity);

		// Create and (empty) Salmon component to be able to refer to all turtles
		registry.renderRequests.insert(
			entity,
			{TEXTURE_ASSET_ID::WALL,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE});
	}
	return 0;
}

int createGround(RenderSystem *renderer)
{

	for (int i = 0; i <= 4; i++)
	{
		for (int j = 0; j <= 4; j++)
		{

			auto entity = Entity();

			Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::RECTANGLE);
			registry.meshPtrs.emplace(entity, &mesh);

			// Setting initial motion values
			Motion &motion = registry.motions.emplace(entity);
			motion.angle = 0.f;
			motion.velocity = {0.f, 0.f};
			motion.scale = {1000, 1000};

			motion.position = {(1000 * i) + 500, (1000 * j) + 500};

			// Create and (empty) Salmon component to be able to refer to all turtles

			registry.renderRequests.insert(
				entity,
				{TEXTURE_ASSET_ID::GROUND_WOOD,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE});
		}
	}

	return 0;
}

Entity createBullet(RenderSystem *renderer, vec2 pos, float angle)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.position.x = motion.position.x + 50*sin(angle) + 35*cos(angle);
	motion.position.y = motion.position.y + 50*-cos(angle) + 35*sin(angle);
	motion.angle = angle;
	motion.scale = {30,30};
	int speed = 2000;
	float y_speed = speed * -cos(angle);
	float x_speed = speed * sin(angle);
		
	motion.velocity = {x_speed, y_speed};

	// registry.colliders.emplace(entity);
	// registry.walls.emplace(entity);

	// Create and (empty) Salmon component to be able to refer to all turtles
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::BULLET, // TEXTURE_COUNT indicates that no txture is needed
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}
