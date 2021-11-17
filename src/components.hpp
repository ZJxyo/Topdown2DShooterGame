#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"

// Player component
struct Player
{
};

struct Bullet
{
	float speed = 1000.f;
};

// Turtles and pebbles have a hard shell
struct Enemy
{
};

// Fish and Salmon have a soft shell
struct SoftShell
{
};

// All data relevant to the shape and motion of entities
struct Motion
{
	vec2 position = {0, 0};
	float angle = 0;
	vec2 velocity = {0, 0};
	vec2 scale = {10, 10};
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity other) { this->other = other; };
};

// Data structure for toggling debug mode
struct Debug
{
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

// Sets the brightness of the screen
struct ScreenState
{
	float darken_screen_factor = -1;
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

// A timer that will be associated to dying salmon
struct DeathTimer
{
	float counter_ms = 3000;
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & salmon.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex> &out_vertices, std::vector<uint16_t> &out_vertex_indices, vec2 &out_size);
	vec2 original_size = {1, 1};
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

struct Health
{
	unsigned int health;
	Health(unsigned int health) : health(health) {}
};

// Collider shapes for collision detection, default box collider
struct PolygonCollider
{
	std::vector<vec3> vertices = {
		{-0.5f, -0.5f, 1.f},
		{0.5f, -0.5f, 1.f},
		{0.5f, 0.5f, 1.f},
		{-0.5f, 0.5f, 1.f}};
};

struct CircleCollider {
	float radius;
	CircleCollider(float radius) : radius(radius) {}
};

struct PointCollider {};

// indicate this is a wall type object
struct Wall
{
};

struct Animate
{
	float counter_ms = 100;

};

struct FireRate
{
	float fire_rate = 0;
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID
{
	FISH = 0,
	TURTLE = FISH + 1,
	PLAYER = TURTLE + 1,
	PLAYER2 = PLAYER + 1,
	PLAYER3 = PLAYER2 + 1,
	PLAYER4 = PLAYER3 + 1,
	PLAYER5 = PLAYER4 + 1,
	PLAYER6 = PLAYER5 + 1,
	PLAYER7 = PLAYER6 + 1,
	GROUND_WOOD = PLAYER7 + 1,
	WALL = GROUND_WOOD + 1,
  BULLET = WALL + 1,
  HELP0 = BULLET + 1,
  HELP1 = HELP0 + 1,
  HELP2 = HELP1 + 1,
  HELP3 = HELP2 + 1,
	TEXTURE_COUNT = HELP3 + 1 
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID
{
	COLOURED = 0,
	PEBBLE = COLOURED + 1,
	SALMON = PEBBLE + 1,
	TURTLE = SALMON + 1,
	TEXTURED = TURTLE + 1,
	WATER = TEXTURED + 1,
    LIGHT = WATER + 1,
	EFFECT_COUNT = LIGHT + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID
{
	SALMON = 0,
	SPRITE = SALMON + 1,
	PEBBLE = SPRITE + 1,
	RECTANGLE = PEBBLE + 1,
	SCREEN_TRIANGLE = RECTANGLE + 1,
	GEOMETRY_COUNT = SCREEN_TRIANGLE + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest
{
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};
