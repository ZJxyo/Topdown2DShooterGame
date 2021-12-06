#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include <map>
#include <list>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

using namespace std;
using namespace nlohmann;
using MyArray = std::array<std::array<int, 50>, 50>;

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

	motion.velocity = {0.f, 0.f};
	motion.scale = {150, 100};

	// Create and (empty) Salmon component to be able to refer to all turtles
	registry.players.emplace(entity);
	Health &health = registry.healths.emplace(entity);
	health.health = 1000;
	registry.avatarColliders.emplace(entity, 50);
	registry.animates.emplace(entity);
	registry.fireRates.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::PLAYER,
		 EFFECT_ASSET_ID::ANIMATE,
		 GEOMETRY_BUFFER_ID::SPRITE});

	registry.renderRequests2.insert(
		entity,
		{TEXTURE_ASSET_ID::FEET,
		 EFFECT_ASSET_ID::ANIMATE,
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

	registry.wallColliders.emplace(entity);
	registry.walls.emplace(entity);
	registry.destroyable.emplace(entity);
	Health &h = registry.healths.emplace(entity);
	h.health = 50;

	// Create and (empty) Salmon component to be able to refer to all turtles
	registry.renderRequests.insert(
			entity,
			{TEXTURE_ASSET_ID::WALL,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}

Entity createStoryBox(RenderSystem *renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::RECTANGLE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({-FISH_BB_WIDTH, FISH_BB_HEIGHT});

//	registry.softShells.emplace(entity);
//    registry.polygonColliders.emplace(entity);
//    registry.walls.emplace(entity);
    registry.storyBox.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::STORY_BOX,
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
	registry.avatarColliders.emplace(entity, 50);

	// Initialize the motion
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0.f, 0.f};
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({100, 100});

	// Create and (empty) Turtle component to be able to refer to all turtles
	registry.enemies.emplace(entity);
	registry.animates.emplace(entity);
	registry.fireRates.emplace(entity);
	Health &health = registry.healths.emplace(entity);
	health.health = 100;
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::PLAYER,
		 EFFECT_ASSET_ID::TURTLE,
		 GEOMETRY_BUFFER_ID::SPRITE});

	registry.renderRequests2.insert(
		entity,
		{TEXTURE_ASSET_ID::FEET,
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

	Motion &motion = registry.motions.emplace(entity);
	motion.angle = angle;
	motion.velocity = {0, 0};

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

Entity createBomb(RenderSystem *renderer, vec2 pos){

	Entity bomb = Entity();
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(bomb, &mesh);
	Bomb &b = registry.bomb.emplace(bomb);
    BombInfo &bombInfo = registry.bombInfo.emplace(bomb);
    bombInfo.position = pos;
	Motion &m = registry.motions.emplace(bomb);
	m.position = pos;
	m.scale = {50.f,80.f};
	m.velocity = {0.f,0.f};
	m.angle = 0.f;
	registry.renderRequests.insert(
			bomb,
			{TEXTURE_ASSET_ID::BOMB,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE});
	return bomb;
}



Entity createEndScreen(RenderSystem *renderer, vec2 pos, bool win, int mode){

	Entity endscreen = Entity();
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(endscreen, &mesh);
	Motion &m = registry.motions.emplace(endscreen);
	m.position = pos;
	m.scale = {600.f,300.f};
	m.velocity = {0.f,0.f};
	m.angle = 0.f;
	if (win && mode == 1)
	registry.renderRequests.insert(
			endscreen,
			{TEXTURE_ASSET_ID::BOMBWIN,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE});
	else if (win && mode == 2)
	registry.renderRequests.insert(
			endscreen,
			{TEXTURE_ASSET_ID::DEFUSEWIN,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE});
	else if (win && mode == 3)
	registry.renderRequests.insert(
			endscreen,
			{TEXTURE_ASSET_ID::ELIMWIN,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE});
	else if (!win && mode == 1)
	registry.renderRequests.insert(
			endscreen,
			{TEXTURE_ASSET_ID::DEFUSEWIN,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE});
	else if (!win && mode == 2)
	registry.renderRequests.insert(
			endscreen,
			{TEXTURE_ASSET_ID::ELIMWIN,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE});
	else if (!win && mode == 3)
	registry.renderRequests.insert(
			endscreen,
			{TEXTURE_ASSET_ID::DEFUSEWIN,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE});

	return endscreen;
}
struct wall
{
	vec2 position;
	float angle;
	vec2 scale;
};

MyArray createMatrix(std::string path) {// matrix 2d array
	
	MyArray T;
	Fill(T);
	

	//load from map.json
	string src = PROJECT_SOURCE_DIR;
	src += path;
	ifstream ifs(src);
	json j;
	ifs >> j;

	for (json q : j["walls"]) {
		for (json w: q["motion"]) {
		int value_x = int(w["position"]["x"]) / 100;
		int value_y = int(w["position"]["y"]) / 100;
		T[value_y][value_x] = 1;

		int scale_x = (((int(w["scale"]["x"]))/100) - 1)/2;
		int scale_y = (((int(w["scale"]["y"]))/100) - 1)/2;
		for (int i = 0; i <= scale_x; i++) {
			for (int j = 0; j<= scale_y; j++ ) {
					T[value_y + j][value_x + i] = 1;
					T[value_y + j][value_x - i] = 1;
					T[value_y - j][value_x + i] = 1;
					T[value_y - j][value_x - i] = 1;
			}
			
		}
		// for (int i = 0;i <= scale_y; i++) {
		// 	T[value_y + i][value_x] = 1;
		// 	T[value_y - i][value_x] = 1;
		// }
		}
		
	}

//	Print(T);
    return T;
}

int SetupMap(RenderSystem *renderer, int current_map)
{
	string src = PROJECT_SOURCE_DIR;
	src += "src/map/map" + to_string(current_map) + ".json";
	ifstream ifs(src);
	json j;
	ifs >> j;

	for (json w : j["plant_spots"]) {
		auto entity = Entity();

		Motion &motion = registry.motions.emplace(entity);
		registry.plantAreas.emplace(entity);

		int pre_value1 = int(w["position"]["x"]);
		int pre_value2 = int(w["position"]["y"]);
		int value1 = pre_value1 + 50;
		int value2 = pre_value2 + 50;

		motion.position = vec2(value1, value2);
		motion.scale = vec2(w["scale"]["x"], w["scale"]["y"]);
		registry.floorRenderRequests.insert(
			entity,
			{TEXTURE_ASSET_ID::TEXTURE_COUNT,
			 EFFECT_ASSET_ID::PLANTSPOT,
			 GEOMETRY_BUFFER_ID::RECTANGLE});
	}
	for (json wall_obj: j["walls"]){
		for (json w : wall_obj["motion"])
		{
			auto entity = Entity();

			// Store a reference to the potentially re-used mesh object
			Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::RECTANGLE);
			registry.meshPtrs.emplace(entity, &mesh);
			registry.wallColliders.emplace(entity);

			// Setting initial motion values
			Motion &motion = registry.motions.emplace(entity);
			int pre_value1 = int(w["position"]["x"]);
			int pre_value2 = int(w["position"]["y"]);
			int value1 = pre_value1 + 50;
			int value2 = pre_value2 + 50;
			motion.position = vec2(value1, value2);
			motion.angle = w["angle"];
			motion.scale = vec2(w["scale"]["x"], w["scale"]["y"]);

			//registry.polygonColliders.emplace(entity);
			registry.walls.emplace(entity);

			// Create and (empty) Salmon component to be able to refer to all turtles
			registry.renderRequests.insert(
				entity,
				{TEXTURE_ASSET_ID::WALL,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE});
		}
	}
	return 0;
}




void Fill(MyArray &T){
    for(auto &row : T){
        for(auto &el : row){
            el = 0;
        }
    }
}

void Print(const MyArray &T){
    for(auto &row : T){
        for(auto &el : row){
            cout<<el<<" ";
        }
        cout << endl;
    }
}



int createGround(RenderSystem *renderer, int current_map)
{
	
	string src = PROJECT_SOURCE_DIR;
	src += "src/map/map" + to_string(current_map) + ".json";
	ifstream ifs(src);
	json j;
	ifs >> j;

	for (json ground_obj: j["ground"]){
		for (json m: ground_obj["motion"])
		{
			auto entity = Entity();

			Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::RECTANGLE);
			registry.meshPtrs.emplace(entity, &mesh);

			// Setting initial motion values
			Motion &motion = registry.motions.emplace(entity);
			motion.angle = 0.f;
			motion.velocity = {0.f, 0.f};
			motion.scale = {m["scale"]["x"], m["scale"]["y"]};

			motion.position = {m["position"]["x"], m["position"]["y"]};

			// Create and (empty) Salmon component to be able to refer to all turtles

			registry.floorRenderRequests.insert(
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
	motion.position.x = motion.position.x + 50 * sin(angle) + 35 * cos(angle);
	motion.position.y = motion.position.y + 50 * -cos(angle) + 35 * sin(angle);
	motion.angle = angle;
	motion.scale = {30, 30};
	int speed = registry.bullets.emplace(entity).speed;
	float y_speed = speed * -cos(angle);
	float x_speed = speed * sin(angle);

	motion.velocity = {x_speed, y_speed};

	registry.bulletColliders.emplace(entity);

	// Create and (empty) Salmon component to be able to refer to all turtles
	registry.bulletsRenderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::BULLET, // TEXTURE_COUNT indicates that no txture is needed
		 EFFECT_ASSET_ID::INSTANCES,
		 GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}

Entity createShockwave(vec2 pos) {
	Entity entity = Entity();
	registry.shockwaveSource.emplace(entity, pos);
	return entity;
}

Entity createLightSource(vec2 pos, std::vector<vec3>& vertices, std::vector<unsigned int>& indices) {
	
	Entity entity = Entity();
	registry.lightSources.emplace(entity, vertices, indices);

	return entity;
}

Entity createNonConvexWall(float thickness, std::vector<vec2>& hinges) {
	float half_thickness = thickness / 2.f;
	std::vector<vec2> vertices;
	std::vector<vec3> verticesV3;
	std::vector<unsigned int> indices;
	for (int i = 0; i < hinges.size() - 1; i++) {
		vec2 h1 = hinges[i];
		vec2 h2 = hinges[i + 1];
		// direction from hinge i to hinge i+1 
		vec2 dir = normalize(h2 - h1);
		vec2 normal = vec2(-dir.y, dir.x);
		// hinge 1 vertex 1
		vec2 v = h1 + (normal - dir) * half_thickness;
		vertices.push_back(v);
		verticesV3.push_back(vec3(v, 1.f));
		// hinge 1 vertex 2
		v = h1 + (-normal - dir) * half_thickness;
		vertices.push_back(v);
		verticesV3.push_back(vec3(v, 1.f));
		// hinge 2 vertex 1
		v = h2 + (normal + dir) * half_thickness;
		vertices.push_back(v);
		verticesV3.push_back(vec3(v, 1.f));
		// hinge 2 vertex 2
		v = h2 + (-normal + dir) * half_thickness;
		vertices.push_back(v);
		verticesV3.push_back(vec3(v, 1.f));
		unsigned int index = (unsigned int)i;
		//triangle 1
		indices.push_back(index * 4);
		indices.push_back(index * 4 + 1);
		indices.push_back(index * 4 + 2);
		//triangle 2
		indices.push_back(index * 4 + 1);
		indices.push_back(index * 4 + 2);
		indices.push_back(index * 4 + 3);
	}

	Entity e = Entity();
	registry.nonConvexWallColliders.emplace(e).vertices = vertices;
	registry.customMeshes.emplace(e, verticesV3, indices).color = vec3(0.678f, 0.847f, 0.902f);
	registry.healths.emplace(e).health = 200;
	registry.customMeshRenderRequests.insert(e,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::COLOURED,
		 GEOMETRY_BUFFER_ID::CUSTOM });
	return e;
}

Entity createParticleSource(uint8 size, float radius, float life_span, vec3 color, vec2 pos, float angle, float speed) {
	assert(size != 0);
	std::vector<vec2> positions;
	std::vector<vec2> velocities;
	for (uint i = 0; i < size; i++) {
		positions.push_back(pos);
		//-0.5 to 0.5
		float random_float = ((float)rand() / (float)RAND_MAX) - 0.5f;
		float angle_span = M_PI / 3.f;
		float random_angle = angle + random_float * angle_span;
		vec2 random_dir = vec2(cos(random_angle), sin(random_angle));
		float random_speed = speed * (1.f + (((float)rand() / (float)RAND_MAX) - 0.5f));
		velocities.push_back(random_dir * random_speed);
	}

	Entity ps = Entity();
	registry.particleSources.emplace(ps, size, radius, life_span, color, positions, velocities);

	return ps;
}

Entity createPushArea(vec2 pos, float dist, float angle, float span) {
	Entity entity = Entity();
	registry.pushAreaColliders.emplace(entity, pos, dist, angle, span);
	return entity;
}