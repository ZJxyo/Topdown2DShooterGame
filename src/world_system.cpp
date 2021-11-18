// Header
#include "world_system.hpp"
#include "world_init.hpp"
#include "ai_system.hpp"
// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"

// AI111
#include "ai_system.hpp"
#include "HelpMenu.h"

// Game configuration
const size_t MAX_TURTLES = 0;
const size_t TURTLE_DELAY_MS = 2000 * 3;
const size_t ANIMATION_DELAY_MS = 100;
const size_t BULLET_TIMER_MS = 100;

// Create the fish world
WorldSystem::WorldSystem()
	: points(0), next_turtle_spawn(0.f), next_fish_spawn(0.f), tap(false)
{
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem()
{
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (salmon_dead_sound != nullptr)
		Mix_FreeChunk(salmon_dead_sound);
	if (salmon_eat_sound != nullptr)
		Mix_FreeChunk(salmon_eat_sound);
	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace
{
	void glfw_err_cb(int error, const char *desc)
	{
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer

GLFWwindow *WorldSystem::create_window()
{

	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int height = mode->height / 6 * 4;
	int width = mode->height;

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(width, height, "Salmon Game Assignment", nullptr, nullptr);
	if (window == nullptr)
	{
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2, int _3)
	{ ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow *wnd, double _0, double _1)
	{ ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_mouse_move({_0, _1}); };
	auto mouse_button_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2)
	{ ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_button_redirect);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
	salmon_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav").c_str());
	salmon_eat_sound = Mix_LoadWAV(audio_path("salmon_eat.wav").c_str());

	if (background_music == nullptr || salmon_dead_sound == nullptr || salmon_eat_sound == nullptr)
	{
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
				audio_path("music.wav").c_str(),
				audio_path("salmon_dead.wav").c_str(),
				audio_path("salmon_eat.wav").c_str());
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem *renderer_arg)
{
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);
	fprintf(stderr, "Loaded music\n");

	// Set all states to default
	restart_game();
}

// AIvy
Entity entity;
// Update our game world

bool WorldSystem::step(float elapsed_ms_since_last_update)
{

	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "Points: " << points;
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());

	//update animation
	Player p = registry.players.get(player_salmon);
	auto &a_entities = registry.animates.entities;
	for (int i = 0; i < registry.animates.components.size(); i++)
	{
		Animate &a = registry.animates.get(a_entities[i]);
		RenderRequest &r = registry.renderRequests.get(a_entities[i]);

		if (a_entities[i] == player_salmon)
		{
			if (length(registry.motions.get(player_salmon).velocity) > 0)
			{
				a.counter_ms -= elapsed_ms_since_last_update * current_speed;
			}
		}
		else
		{
			a.counter_ms -= elapsed_ms_since_last_update * current_speed;
		}
		if (a.counter_ms < 0)
		{
			a.counter_ms = ANIMATION_DELAY_MS;
			if (registry.players.has(a_entities[i]) || registry.enemies.has(a_entities[i]))
			{
				if (a.sprite_frame == a.player_frames -1){
					a.sprite_frame = 0;
				} else {
					a.sprite_frame += 1;
				}
			}
			if (registry.renderRequests2.has(a_entities[i]))
			{
				if (a.sprite_frame == a.feet_frames -1){
					a.sprite_frame = 0;
				} else {
					a.sprite_frame += 1;
				}
			}
		}
	}

	// Spawning new turtles

	next_turtle_spawn -= elapsed_ms_since_last_update * current_speed;
	if (registry.enemies.components.size() <= MAX_TURTLES && next_turtle_spawn < 0.f)
	{
		// Reset timer
		next_turtle_spawn = (TURTLE_DELAY_MS / 2) + uniform_dist(rng) * (TURTLE_DELAY_MS / 2);
		// Create turtle
		entity = createTurtle(renderer, {1000, 1000});
		// Setting random initial position and constant velocity

		Motion &motion = registry.motions.get(entity);
		motion.position =
			vec2(window_width_px - 200.f,
				 50.f + uniform_dist(rng) * (window_height_px - 100.f));

		motion.velocity = vec2(10.f, 10.f);
	}

	// AIvy
	Chase chase(player_salmon);
	ShootNBullets shoot(player_salmon, renderer);
	Build build(player_salmon);
	BTIfCondition btIfCondition(&chase, &shoot, &build);
	btIfCondition.init(entity);
	btIfCondition.process(entity);

	// process shooting bullets for player

	FireRate &fireRate = registry.fireRates.get(player_salmon);
	fireRate.fire_rate -= elapsed_ms_since_last_update * current_speed;

	if (fireRate.fire_rate < 0)
	{
		fireRate.fire_rate = BULLET_TIMER_MS;
		if (mouse_down || tap) {
			Player& player = registry.players.get(player_salmon);
			Motion& motion = registry.motions.get(player_salmon);
			if (tap)
				tap = !tap;


			if (length(motion.velocity) > 0) {
				float LO = -0.5;
				float HI = 0.5;
				float r3 = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));

				createBullet(renderer, motion.position, motion.angle + 1.5708 + r3);
			}

			else {
				createBullet(renderer, motion.position, motion.angle + 1.5708);
			}
		}
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A3: HANDLE PEBBLE SPAWN HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 3
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Processing the salmon state
	assert(registry.screenStates.components.size() <= 1);
	ScreenState &screen = registry.screenStates.components[0];

	float min_counter_ms = 3000.f;
	for (Entity entity : registry.deathTimers.entities)
	{
		// progress timer
		DeathTimer &counter = registry.deathTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if (counter.counter_ms < min_counter_ms)
		{
			min_counter_ms = counter.counter_ms;
		}

		// restart the game once the death timer expired
		if (counter.counter_ms < 0)
		{
			registry.deathTimers.remove(entity);
			screen.darken_screen_factor = 0;
			restart_game();
			return true;
		}
	}
	// reduce window brightness if any of the present salmons is dying
	screen.darken_screen_factor = 1 - min_counter_ms / 3000;

	// !!! TODO A1: update LightUp timers and remove if time drops below zero, similar to the death counter

	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game()
{
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// Reset the game speed
	current_speed = 1.f;
	mouse_down = false;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all fish, turtles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

	// Debugging for memory/component leaks
	registry.list_all_components();

	// create ground
	createGround(renderer);

	// Create a new salmon
	player_salmon = createSalmon(renderer, {1000, 1000});
	registry.colors.insert(player_salmon, {1, 0.8f, 0.8f});

	SetupMap(renderer);
	createMatrix();
	createWall(renderer, {300, 300}, 2.f, {200, 200});

	// CLEAN

	//createWall(renderer, { 300, 100 }, 0.f, { 200, 200 });
	//createWall(renderer, { 900, 100 }, 0.f, { 200, 200 });
	//createWall(renderer, { 300, 500 }, 0.f, { 200, 200 });
	//createWall(renderer, { 700, 500 }, 0.f, { 200, 200 });
	//createWall(renderer, { 1100, 500 }, 0.f, { 200, 200 });
	//createWall(renderer, { 1100, 700 }, 0.f, { 200, 200 });
}

// Compute collisions between entities
//void WorldSystem::handle_collisions()
//{
//	// Loop over all collisions detected by the physics system
//	auto &collisionsRegistry = registry.collisions; // TODO: @Tim, is the reference here needed?
//	for (uint i = 0; i < collisionsRegistry.components.size(); i++)
//	{
//		// The entity and its collider
//		Entity entity = collisionsRegistry.entities[i];
//		Entity entity_other = collisionsRegistry.components[i].other;
//
//		// For now, we are only interested in collisions that involve the salmon
//		if (registry.players.has(entity))
//		{
//			//Player& player = registry.players.get(entity);
//
//			// Checking Player - HardShell collisions
//			if (registry.enemies.has(entity_other))
//			{
//				// initiate death unless already dying
//				//if (!registry.deathTimers.has(entity)) {
//				//	// Scream, reset timer, and make the salmon sink
//				//	registry.deathTimers.emplace(entity);
//				//	Mix_PlayChannel(-1, salmon_dead_sound, 0);
//				//	registry.motions.get(entity).angle = 3.1415f;
//				//	registry.motions.get(entity).velocity = { 0, 80 };
//				/*assert(registry.healths.has(entity));
//				Health& player_health = registry.healths.get(entity);
//				player_health.health -= 1;
//				printf("Health: %d", player_health.health);*/
//			}
//			// Checking Player - SoftShell collisions
//			else if (registry.softShells.has(entity_other))
//			{
//				if (!registry.deathTimers.has(entity))
//				{
//					// chew, count points, and set the LightUp timer
//					registry.remove_all_components_of(entity_other);
//					Mix_PlayChannel(-1, salmon_eat_sound, 0);
//					++points;
//
//					// !!! TODO A1: create a new struct called LightUp in components.hpp and add an instance to the salmon entity by modifying the ECS registry
//				}
//			}
//		}
//	}
//
//	// Remove all collisions from this simulation step
//	registry.collisions.clear();
//}

// Should the game be over ?
bool WorldSystem::is_over() const
{
	return bool(glfwWindowShouldClose(window));
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// key is of 'type' GLFW_KEY_
	// action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		restart_game();
	}

	// Debugging
	if (key == GLFW_KEY_C)
	{
		if (action == GLFW_RELEASE)
			debugging.in_debug_mode = false;
		else
			debugging.in_debug_mode = true;
	}

	// Player movment WASD

	if (!registry.deathTimers.has(player_salmon))
	{
		if (action == GLFW_PRESS)
		{
			if (key == GLFW_KEY_W)
			{
				input.up = 1.f;
				update_player_velocity();
			}
			else if (key == GLFW_KEY_S)
			{
				input.down = 1.f;
				update_player_velocity();
			}
			else if (key == GLFW_KEY_A)
			{
				input.left = 1.f;
				update_player_velocity();
			}
			else if (key == GLFW_KEY_D)
			{
				input.right = 1.f;
				update_player_velocity();
			}
		}
		if (action == GLFW_RELEASE)
		{
			if (key == GLFW_KEY_W)
			{
				input.up = 0;
				update_player_velocity();
			}
			else if (key == GLFW_KEY_S)
			{
				input.down = 0;
				update_player_velocity();
			}
			else if (key == GLFW_KEY_A)
			{
				input.left = 0;
				update_player_velocity();
			}
			else if (key == GLFW_KEY_D)
			{
				input.right = 0;
				update_player_velocity();
			}
		}
	}

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA)
	{
		current_speed -= 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD)
	{
		current_speed += 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	current_speed = fmax(0.f, current_speed);
}

void WorldSystem::on_mouse_move(vec2 mouse_position)
{

	Motion &motion = registry.motions.get(player_salmon);

	int w, h;
	glfwGetWindowSize(window, &w, &h);

	float angle = atan2(mouse_position.y - h / 2.f, mouse_position.x - w / 2.f);

	motion.angle = angle;
}

void WorldSystem::on_mouse_click(int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		mouse_down = true;
		tap = true;
	}
	else if (action == GLFW_RELEASE)
	{
		mouse_down = false;
	}
}

void WorldSystem::handle_collision(Entity entity_1, Entity entity_2) {
	if (registry.healths.has(entity_1) && registry.bullets.has(entity_2)) {
		registry.healths.get(entity_1).health -= 1;

		printf("HP - 10\n");
	}
	else if (registry.healths.has(entity_2) && registry.bullets.has(entity_1))
	{
		registry.healths.get(entity_2).health -= 1;
		printf("HP - 10\n");
	}
}

void WorldSystem::update_player_velocity() {
	registry.motions.get(player_salmon).velocity = player_speed * vec2(input.right - input.left, input.down - input.up);
}

Entity WorldSystem::createParticleSource(uint8 size, float radius, float decay, vec3 color, vec2 pos, vec2 vel) {
	assert(size != 0);
	std::vector<vec2> positions;
	std::vector<vec2> velocities;
	float speed = length(vel);
	vec2 dir = normalize(vel);
	for (uint i = 0; i < size; i++) {
		positions.push_back(pos);
		// -0.5 to 0.5
		float random_float = (uniform_dist(rng) - 0.5);
		float cs = cos(random_float * M_PI);
		float sn = sin(random_float * M_PI);
		vec2 random_dir = vec2(dir.x * cs - dir.y * sn, dir.x * sn + dir.y * cs);
		float random_speed = speed * (1 + 0.5 * (uniform_dist(rng) - 0.5));
		velocities.push_back(random_dir * random_speed);
	}

	Entity ps = Entity();
	registry.particleSources.emplace(ps, size, radius, decay, color, positions, velocities);

	return ps;
}