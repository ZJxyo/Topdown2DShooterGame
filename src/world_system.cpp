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
const size_t BOMB_TIMER_MS = 40000.f;
const size_t FOOTSTEPS_SOUND_TIMER_MS = 400.f;
int toggle[4] = {-1, -1, -1, -1};
Entity stories[4];
Entity boxes[4];
vec2 oldPosition;

// Create the fish world
WorldSystem::WorldSystem()
	: points(0), next_turtle_spawn(0.f), next_fish_spawn(0.f), tap(false), can_plant(false),
	plant_timer(2000.0f), explode_timer(BOMB_TIMER_MS), bomb_planted(false), is_planting(false), bomb_exploded(false),footsteps_timer(FOOTSTEPS_SOUND_TIMER_MS)

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
	if (bomb_planted_sound != nullptr)
		Mix_FreeChunk(bomb_planted_sound);
	if (bomb_planting_sound != nullptr)
		Mix_FreeChunk(bomb_planting_sound);
	if (bomb_countdown_sound != nullptr)
		Mix_FreeChunk(bomb_countdown_sound);
	if (bomb_explosion_sound != nullptr)
		Mix_FreeChunk(bomb_explosion_sound);
	if (footsteps_sound != nullptr)
		Mix_FreeChunk(footsteps_sound);
	
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
	bomb_planted_sound = Mix_LoadWAV(audio_path("bomb_planted.wav").c_str());
	bomb_planting_sound = Mix_LoadWAV(audio_path("bomb_planting.wav").c_str());
	bomb_countdown_sound = Mix_LoadWAV(audio_path("bomb_countdown.wav").c_str());
	bomb_explosion_sound = Mix_LoadWAV(audio_path("bomb_explosion.wav").c_str());
	footsteps_sound = Mix_LoadWAV(audio_path("footsteps.wav").c_str());

	if (background_music == nullptr || salmon_dead_sound == nullptr || salmon_eat_sound == nullptr || bomb_planted_sound == nullptr||bomb_planting_sound == nullptr
	|| bomb_countdown_sound == nullptr || bomb_explosion_sound == nullptr || footsteps_sound == nullptr)
	{
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
				audio_path("music.wav").c_str(),
				audio_path("salmon_dead.wav").c_str(),
				audio_path("salmon_eat.wav").c_str()),
				audio_path("bomb_planted.wav").c_str(),
				audio_path("bomb_planting.wav").c_str(),
				audio_path("bomb_countdown.wav").c_str(),
				audio_path("bomb_explosion.wav").c_str(),
				audio_path("footsteps.wav").c_str()
				;
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem *renderer_arg)
{
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	// Mix_PlayMusic(background_music, -1);
	// fprintf(stderr, "Loaded music\n");

	// Set all states to default
	restart_game();
}

// AIvy for turtle
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

	if(bomb_exploded){
		return true;
	}

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

    // show storybox 1
    if(abs(registry.motions.get(player_salmon).position.x -  BOX1_LOCATION.x)  < 50
    && abs(registry.motions.get(player_salmon).position.y - BOX1_LOCATION.y) < 50) {

        if(toggle[0] == -1) {
            oldPosition = registry.motions.get(entity).position;
            stories[0] = helpMenu.createStroy1(renderer, window, { BOX1_LOCATION.x,BOX1_LOCATION.y });
            registry.motions.get(entity).position = {2000, 2000};
            registry.motions.get(entity).velocity = {0,0};
            toggle[0] = 0;
        }

        if (!helpMenu.showStory1 && toggle[0] == 0) {
            registry.remove_all_components_of(stories[0]);
            toggle[0] = 1;
            if(toggle[0] == 1) {
                registry.motions.get(entity).position = oldPosition;
            }
        }
    }

    // show storybox 2
    if(abs(registry.motions.get(player_salmon).position.x -  BOX2_LOCATION.x)  < 50
       && abs(registry.motions.get(player_salmon).position.y - BOX2_LOCATION.y) < 50) {

        if(toggle[1] == -1) {
            oldPosition = registry.motions.get(entity).position;
            stories[1] = helpMenu.createStroy2(renderer, window, { BOX2_LOCATION.x,BOX2_LOCATION.y });
            registry.motions.get(entity).position = {2000, 2000};
            registry.motions.get(entity).velocity = {0,0};
            toggle[1] = 0;
        }

        if (!helpMenu.showStory2&& toggle[1] == 0) {
            registry.remove_all_components_of(stories[1]);
            toggle[1] = 1;
            if(toggle[1] == 1) {
                registry.motions.get(entity).position = oldPosition;
            }
        }
    }

    // show storybox 3
    if(abs(registry.motions.get(player_salmon).position.x -  BOX3_LOCATION.x)  < 50
       && abs(registry.motions.get(player_salmon).position.y - BOX3_LOCATION.y) < 50) {

        if(toggle[2] == -1) {
            oldPosition = registry.motions.get(entity).position;
            stories[2] = helpMenu.createStroy3(renderer, window, { BOX3_LOCATION.x,BOX3_LOCATION.y });
            registry.motions.get(entity).position = {2000, 2000};
            registry.motions.get(entity).velocity = {0,0};
            toggle[2] = 0;
        }

        if (!helpMenu.showStory3 && toggle[2] == 0) {
            registry.remove_all_components_of(stories[2]);
            toggle[2] = 1;
            if(toggle[2] == 1) {
                registry.motions.get(entity).position = oldPosition;
            }
        }
    }

    // show storybox 4
    if(abs(registry.motions.get(player_salmon).position.x -  BOX4_LOCATION.x)  < 50
       && abs(registry.motions.get(player_salmon).position.y - BOX4_LOCATION.y) < 50) {

        if(toggle[3] == -1) {
            oldPosition = registry.motions.get(entity).position;
            stories[3] = helpMenu.createStroy4(renderer, window, { BOX4_LOCATION.x,BOX4_LOCATION.y });
            registry.motions.get(entity).position = {2000, 2000};
            registry.motions.get(entity).velocity = {0,0};
            toggle[3] = 0;
        }

        if (!helpMenu.showStory4 && toggle[3] == 0) {
            registry.remove_all_components_of(stories[3]);
            toggle[3] = 1;
            if(toggle[3] == 1) {
                registry.motions.get(entity).position = oldPosition;
            }
        }
    }



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
	

	Motion &motion = registry.motions.get(player_salmon);
	// check if moving
	if (motion.velocity.x != 0 || motion.velocity.y !=  0) {

		
		footsteps_timer -= elapsed_ms_since_last_update * current_speed;
		if (footsteps_timer < 0) {
				Mix_PlayChannel(-1, footsteps_sound, 0);
				footsteps_timer = FOOTSTEPS_SOUND_TIMER_MS;
		}
	}

	// check if player is in designated area to plant bomb
	int player_x = motion.position.x ;
	int player_y = motion.position.y ;





	auto &pa_entities = registry.plantAreas.entities;
	for (int i = 0; i < registry.plantAreas.components.size(); i++)
	{
		PlantArea &pa = registry.plantAreas.get(pa_entities[i]);
		Motion &m = registry.motions.get(pa_entities[i]);
		if (player_x > (m.position.x - ((m.scale.x - 100) / 2)) && 
				player_x < (m.position.x + ((m.scale.x - 100) / 2))
				&& player_y > (m.position.y - ((m.scale.y - 100) / 2)) &&
				player_y < (m.position.y + ((m.scale.y - 100) / 2)))
				 {
				
					can_plant = true;
					break;

			}  else {
				can_plant = false;
			}

	}

	if (is_planting && !bomb_planted){
		plant_timer -= elapsed_ms_since_last_update * current_speed;
	}

	if (plant_timer < 0 && !bomb_planted) {
		cout << "planted";
		createBomb(renderer,motion.position);
		Mix_PlayChannel(-1, bomb_planted_sound, 0);
		is_planting = false;
		bomb_planted = true;
		
	} 

	if (bomb_planted) {
		if (explode_timer == BOMB_TIMER_MS) {
			Mix_PlayChannel(-1, bomb_countdown_sound, 0);	
		}
		explode_timer -= elapsed_ms_since_last_update * current_speed;
		
		
	}

	if (explode_timer < 0 && !bomb_exploded) {
		cout << "explode";
		bomb_exploded = true;
		
		Mix_PlayChannel(-1, bomb_explosion_sound, 0);
		
		createEndScreen(renderer,motion.position);
	}

	
	
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
	float time = elapsed_ms_since_last_update / 1000.f;

	for (int i = registry.particleSources.entities.size() - 1; i >= 0; i--) {
		ParticleSource& ps = registry.particleSources.components[i];
		ps.alpha -= ps.decay * time;
		if (ps.alpha <= 0.5) {
			registry.remove_all_components_of(registry.particleSources.entities[i]);
			continue;
		}
		for (int j = 0; j < ps.size; j++) {
			ps.positions[j] += ps.velocities[j] * time;
		}
	}

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
	//createWall(renderer, {300, 300}, 2.f, {200, 200});

    // create story box
    boxes[0] = createStoryBox(renderer, BOX1_LOCATION);
    boxes[1] = createStoryBox(renderer, BOX2_LOCATION);
    boxes[2] = createStoryBox(renderer, BOX3_LOCATION);
    boxes[3] = createStoryBox(renderer, BOX4_LOCATION);

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

    if (action == GLFW_PRESS && key == GLFW_KEY_SPACE && helpMenu.showStory1 && toggle[0] == 0)
    {
        helpMenu.showStory1 = false;
        registry.remove_all_components_of(boxes[0]);

    } else if (action == GLFW_PRESS && key == GLFW_KEY_SPACE && helpMenu.showStory2 && toggle[1] == 0)
    {
        helpMenu.showStory2 = false;
        registry.remove_all_components_of(boxes[1]);

    } else if (action == GLFW_PRESS && key == GLFW_KEY_SPACE && helpMenu.showStory3 && toggle[2] == 0)
    {
        helpMenu.showStory3 = false;
        registry.remove_all_components_of(boxes[2]);

    } else if (action == GLFW_PRESS && key == GLFW_KEY_SPACE && helpMenu.showStory4 && toggle[3] == 0)
    {
        helpMenu.showStory4 = false;
        registry.remove_all_components_of(boxes[3]);
    }

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		restart_game();
	}

	if(bomb_exploded){
		input.up = 0;
		input.down = 0;
		input.left = 0;
		input.right = 0;
		update_player_velocity();
		return;
	}

	// Debugging
	if (key == GLFW_KEY_C)
	{
		if (action == GLFW_RELEASE)
			debugging.in_debug_mode = false;
		else
			debugging.in_debug_mode = true;
	}

	// Player planting bomb

	if (!registry.deathTimers.has(player_salmon)) {
		if (key == GLFW_KEY_E) {

			if (!bomb_planted){
				if (action == GLFW_PRESS) {
					if (can_plant ) {
						is_planting = true;
						cout << "planting";
						Mix_PlayChannel(-1, bomb_planting_sound, 0);
						
					
					} 

				}
				else if (action == GLFW_RELEASE) {
					plant_timer = 5000.0f;
					is_planting = false;
					//cout << "plant release";
				}
			}
		}
		
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

		if (is_planting) {
			input.up = 0;
			input.down = 0;
			input.left = 0;
			input.right = 0;
			update_player_velocity();
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

// e1 should be the bullet
void WorldSystem::handle_bullet_hit(Entity bullet, Entity entity) {
	if (registry.healths.has(entity)) {
		registry.healths.get(entity).health -= 1;
	}

	assert(registry.motions.has(bullet));
	Motion& bullet_motion = registry.motions.get(bullet);
	createParticleSource(20, 3.f, 1.5f, vec3(1.f, 0.f, 0.f), bullet_motion.position, -normalize(bullet_motion.velocity), 300.f);

	if (registry.shockwaveSource.size() == 0) {
		createShockwave(bullet_motion.position);
	}
}

void WorldSystem::update_player_velocity() {
	registry.motions.get(player_salmon).velocity = player_speed * vec2(input.right - input.left, input.down - input.up);
}

Entity WorldSystem::createParticleSource(uint8 size, float radius, float decay, vec3 color, vec2 pos, vec2 dir, float speed) {
	assert(size != 0);
	std::vector<vec2> positions;
	std::vector<vec2> velocities;
	for (uint i = 0; i < size; i++) {
		positions.push_back(pos);
		 //-0.5 to 0.5
		float random_float = ((float)rand() / (float)RAND_MAX) - 0.5f;
		float cs = cos(random_float * M_PI / 2.f);
		float sn = sin(random_float * M_PI / 2.f);
		vec2 random_dir = vec2(dir.x * cs - dir.y * sn, dir.x * sn + dir.y * cs);
		float random_speed = speed * (1.f + (((float)rand() / (float)RAND_MAX) - 0.5f));
		velocities.push_back(random_dir * random_speed);
	}

	Entity ps = Entity();
	registry.particleSources.emplace(ps, size, radius, decay, color, positions, velocities);

	return ps;
}