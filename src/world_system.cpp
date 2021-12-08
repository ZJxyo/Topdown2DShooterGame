// Header
#include "world_system.hpp"
#include "world_init.hpp"
#include "ai_system.hpp"
// stlib
#include <cassert>
#include <sstream>

#include <nlohmann/json.hpp>

#include "physics_system.hpp"

// AI111
#include "ai_system.hpp"
#include "HelpMenu.h"

using namespace nlohmann;

// Game configuration
const size_t MAX_TURTLES = 5;
const size_t CHASE_DELAY_MS = 250;
const size_t ANIMATION_DELAY_MS = 100;
const size_t BULLET_TIMER_MS = 100;
const size_t BOMB_TIMER_MS = 40000.f;
const size_t FOOTSTEPS_SOUND_TIMER_MS = 400.f;
const size_t PLANT_TIMER_MS = 2000.0f;
const size_t ITEM_RESPAWN_DELAY = 30000.f;
const size_t WALL_COOLDOWN = 2000.f;
const size_t DEFUSE_TIMER_MS = 6000.0f;
const size_t PUSH_COOLDOWN = 1000.0f;
json j;

// Create the fish world
WorldSystem::WorldSystem()
	: points(0), next_turtle_spawn(0.f), next_fish_spawn(0.f), tap(false), can_plant(false),
	plant_timer(PLANT_TIMER_MS), explode_timer(BOMB_TIMER_MS), bomb_planted(false), is_planting(false),
	 win_game(false),footsteps_timer(FOOTSTEPS_SOUND_TIMER_MS), buildmode(false), buildcoord({0,0}),
	  mousecoord({0,0}), building(false), maxWall(10), attack_mode(true), defuse_timer(DEFUSE_TIMER_MS),
	  attack_side(0),is_defusing(false),next_chase(0.f),current_map(1), end_screen(false)

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
	if (ak47_sound != nullptr) 
		Mix_FreeChunk(ak47_sound);
	if (defuse_sound != nullptr) 
		Mix_FreeChunk(defuse_sound);
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
	ak47_sound = Mix_LoadWAV(audio_path("ak47.wav").c_str());
	defuse_sound = Mix_LoadWAV(audio_path("defuse.wav").c_str());

	if (background_music == nullptr || salmon_dead_sound == nullptr || salmon_eat_sound == nullptr || bomb_planted_sound == nullptr||bomb_planting_sound == nullptr
	|| bomb_countdown_sound == nullptr || bomb_explosion_sound == nullptr || footsteps_sound == nullptr || ak47_sound == nullptr || defuse_sound == nullptr)
	{
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
				audio_path("music.wav").c_str(),
				audio_path("salmon_dead.wav").c_str(),
				audio_path("salmon_eat.wav").c_str()),
				audio_path("bomb_planted.wav").c_str(),
				audio_path("bomb_planting.wav").c_str(),
				audio_path("bomb_countdown.wav").c_str(),
				audio_path("bomb_explosion.wav").c_str(),
				audio_path("footsteps.wav").c_str(),
				audio_path("ak47.wav").c_str(),
				audio_path("defuse_sound.wav").c_str()
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


	update_player_velocity();

	if(win_game || !canMove){
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

	// AI
	ShootNBullets shoot(player_salmon, renderer, elapsed_ms_since_last_update);
	Build build(player_salmon);
	Guard guard(player_salmon, renderer, elapsed_ms_since_last_update);

	for(int i = 0; i < registry.enemies.entities.size(); i++ ){
		Enemy &e = registry.enemies.components[i];
		Motion m = registry.motions.get(registry.enemies.entities[i]);
		bool defuser = false;
		if (bomb_planted && attack_mode){
			e.guard_mode = false;
			vec2 bomb_pos = registry.motions.get(registry.bomb.entities[0]).position;
			if (i == 0){
				e.guard_mode = true;
				e.pos = bomb_pos;
				defuser = true;
				int distance = sqrt(pow(m.position.x - bomb_pos.x, 2) + pow(m.position.y - bomb_pos.y, 2));
				if (distance < 125){
					if (!is_defusing){
						Mix_PlayChannel(-1, defuse_sound, 0);
					}
					is_defusing = true;
				} else {
					is_defusing = false;
					defuse_timer=DEFUSE_TIMER_MS;
				}
			}
		}
		if (!attack_mode){
			vec2 site_pos = {j["plant_spots"][attack_side]["position"]["x"], j["plant_spots"][attack_side]["position"]["y"]};
			if (i == 0 && !bomb_planted){
				e.pos = site_pos;
			}
			if (i == 0 && is_defusing){
				e.guard_mode = false;
			}
			int distance = sqrt(pow(m.position.x - site_pos.x, 2) + pow(m.position.y - site_pos.y, 2));
			if (distance < 100){
				is_planting = true;
				plant_timer -= elapsed_ms_since_last_update * current_speed;
			}
		}
		if (registry.enemies.components[i].guard_mode){
            Move move(registry.enemies.components[i].pos, current_map);
			BTIfCondition btIfCondition(NULL, &shoot, &build, &guard, &move, defuser);
			Entity entity = registry.enemies.entities[i];
			btIfCondition.process(entity);

			
		} else {
            Chase chase(player_salmon, current_map);
			BTIfCondition btIfCondition(&chase, &shoot, &build, &guard,NULL, defuser);
			Entity entity = registry.enemies.entities[i];
			btIfCondition.init(entity);
			btIfCondition.process(entity);
		}
	}

	next_chase -= elapsed_ms_since_last_update * current_speed;

	// if (next_chase < 0.f) {
	// 	next_chase = CHASE_DELAY_MS;
	// 	Chase chase(player_salmon);
	// 	Build build(player_salmon);
	// 	BTIfCondition btIfCondition(&chase, &shoot, &build);
	// 	btIfCondition.init(entity);
	// 	btIfCondition.process(entity);
	// }
    // show storybox 1
    if(abs(registry.motions.get(player_salmon).position.x -  BOX1_LOCATION.x)  < 50
    && abs(registry.motions.get(player_salmon).position.y - BOX1_LOCATION.y) < 50) {

        if(toggle[0] == -1) {
            tutorials[0] = helpMenu.createTutorial1(renderer, window, {BOX1_LOCATION.x, BOX1_LOCATION.y});
            toggle[0] = 0;
            canMove = false;
            registry.motions.get(player_salmon).velocity = {0,0};
        }

        if (!helpMenu.showTutorial1 && toggle[0] == 0) {
            registry.remove_all_components_of(tutorials[0]);
            toggle[0] = 1;
            registry.motions.get(player_salmon).velocity = {0,0};
            canMove = true;
            input.right = 0;
            input.left = 0;
            input.down = 0;
            input.up = 0;
        }
    }

    // show storybox 2
    if(abs(registry.motions.get(player_salmon).position.x -  BOX2_LOCATION.x)  < 50
       && abs(registry.motions.get(player_salmon).position.y - BOX2_LOCATION.y) < 50) {

        if(toggle[1] == -1) {
            tutorials[1] = helpMenu.createTutorial2(renderer, window, {BOX2_LOCATION.x, BOX2_LOCATION.y});
            toggle[1] = 0;
            canMove = false;
            registry.motions.get(player_salmon).velocity = {0,0};
        }

        if (!helpMenu.showTutorial2 && toggle[1] == 0) {
            registry.remove_all_components_of(tutorials[1]);
            toggle[1] = 1;
            registry.motions.get(player_salmon).velocity = {0,0};
            canMove = true;
            input.right = 0;
            input.left = 0;
            input.down = 0;
            input.up = 0;
        }
    }

    // show storybox 3
    if(abs(registry.motions.get(player_salmon).position.x -  BOX3_LOCATION.x)  < 50
       && abs(registry.motions.get(player_salmon).position.y - BOX3_LOCATION.y) < 50) {

        if(toggle[2] == -1) {
            tutorials[2] = helpMenu.createTutorial3(renderer, window, {BOX3_LOCATION.x, BOX3_LOCATION.y});
            toggle[2] = 0;
            canMove = false;
            registry.motions.get(player_salmon).velocity = {0,0};
        }

        if (!helpMenu.showTutorial3 && toggle[2] == 0) {
            registry.remove_all_components_of(tutorials[2]);
            toggle[2] = 1;
            registry.motions.get(player_salmon).velocity = {0,0};
            canMove = true;
            input.right = 0;
            input.left = 0;
            input.down = 0;
            input.up = 0;
        }
    }

    // show storybox 4
    if(abs(registry.motions.get(player_salmon).position.x -  BOX4_LOCATION.x)  < 50
       && abs(registry.motions.get(player_salmon).position.y - BOX4_LOCATION.y) < 50) {

        if(toggle[3] == -1) {
            tutorials[3] = helpMenu.createTutorial4(renderer, window, {BOX4_LOCATION.x, BOX4_LOCATION.y});
            toggle[3] = 0;
            canMove = false;
            registry.motions.get(player_salmon).velocity = {0,0};
        }

        if (!helpMenu.showTutorial4 && toggle[3] == 0) {
            registry.remove_all_components_of(tutorials[3]);
            toggle[3] = 1;
            registry.motions.get(player_salmon).velocity = {0,0};
            canMove = true;
            input.right = 0;
            input.left = 0;
            input.down = 0;
            input.up = 0;
        }
    }



	// process shooting bullets for player

	FireRate &fireRate = registry.fireRates.get(player_salmon);
	fireRate.fire_rate -= elapsed_ms_since_last_update * current_speed;

	if (fireRate.fire_rate < 0)
	{
		fireRate.fire_rate = BULLET_TIMER_MS;
		if ((left_mouse_down || tap ) && !buildmode) {
			Player& player = registry.players.get(player_salmon);
			Motion& motion = registry.motions.get(player_salmon);
			if (tap)
				tap = !tap;


			if (length(motion.velocity) > 0) {
				float LO = -0.5;
				float HI = 0.5;
				float r3 = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));

				createBullet(renderer, motion.position, motion.angle + 1.5708 + r3);
				Mix_PlayChannel(-1, ak47_sound, 0);

			}

			else {
				createBullet(renderer, motion.position, motion.angle + 1.5708);
				Mix_PlayChannel(-1, ak47_sound, 0);
			}
		}
	}

	//if (buildmode) {
	//	vec2 pos = registry.motions.get(player_salmon).position;
	//	int w, h;
	//	glfwGetWindowSize(window, &w, &h);
	//	if (left_mouse_down && !building){
	//		mousecoord.x -= w/2;
	//		mousecoord.y -= h/2;
	//		mousecoord.x = mousecoord.x * 0.9;
	//		mousecoord.y = mousecoord.y * 0.9;
	//		buildcoord = pos + mousecoord;
	//		building = true;
	//	}
	//	if (building && !left_mouse_down) {
	//		building = false;
	//		buildmode = false;
	//		float angle = atan2(pos.x + mousecoord.x - (w/2) - buildcoord.x, -(pos.y + mousecoord.y - (h/2)- buildcoord.y));
	//		createWall(renderer, buildcoord, angle,{100,300});
	//		maxWall -= 1;
	//	}
	//} 
	

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
	
	if (is_planting && !bomb_planted && attack_mode){
		plant_timer -= elapsed_ms_since_last_update * current_speed;
	}
	
	if (is_defusing && bomb_planted){
		defuse_timer -= elapsed_ms_since_last_update * current_speed;
	}

	if (defuse_timer < 0 && bomb_planted && !attack_mode){
		end_screen = true;
		canMove = false;
		plant_timer=PLANT_TIMER_MS;
		explode_timer=BOMB_TIMER_MS;
		defuse_timer=DEFUSE_TIMER_MS;
		bomb_planted=false;
		is_planting=false;
		is_defusing=false;
		createEndScreen(renderer,motion.position, true, 2);
		return true;
	}

	if (plant_timer < 0 && !bomb_planted &&  attack_mode) {
		cout << "planted";
		vec2 pos = motion.position;
		pos.x = (int) (pos.x +50) / 100 * 100;
		pos.y = (int) (pos.y +50) / 100 * 100;
		createBomb(renderer,pos);
		Mix_PlayChannel(-1, bomb_planted_sound, 0);
		is_planting = false;
		bomb_planted = true;
        // registry.bombInfo.get(bomb).isPlanted = true;
	} 

	if (plant_timer < 0 && !bomb_planted &&  !attack_mode) {
		cout << "planted";
		vec2 site_pos = {j["plant_spots"][attack_side]["position"]["x"], j["plant_spots"][attack_side]["position"]["y"]};
		createBomb(renderer, site_pos);
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

	if (bomb_planted && attack_mode && defuse_timer < 0){
		end_screen = true;
		canMove = false;
		bomb_planted = false;
		createEndScreen(renderer,motion.position, false, 2);
	}

	if (explode_timer < 0 && !win_game && attack_mode) {
		cout << "explode";
		win_game = true;
		
		Mix_PlayChannel(-1, bomb_explosion_sound, 0);
		end_screen = true;
		canMove = false;
		createEndScreen(renderer,motion.position, true, 1);
	}
	
	if (explode_timer < 0 && !win_game && !attack_mode) {
		Mix_PlayChannel(-1, bomb_explosion_sound, 0);
		
		end_screen = true;
		canMove = false;
		createEndScreen(renderer,motion.position, false, 1);
	}
	if (registry.enemies.entities.size() > 0){
		//cout << "x: "<< registry.motions.get(registry.enemies.entities[0]).position.x << "y: " << registry.motions.get(registry.enemies.entities[0]).position.y;
		
	}
	
	if (registry.enemies.entities.size() == 0){
		win_game = true;
		end_screen = true;
		canMove = false;
		createEndScreen(renderer,motion.position, true, 3);
	}


	//// checking HP for player
	//Health &health = registry.healths.get(player_salmon);
	//if (health.health < 0) {
	//	restart_game();
	//}

	//// checking HP for AI turtle
	//for (Entity entity : registry.enemies.entities) {
	//	Health &health = registry.healths.get(entity);
	//	if (health.health < 0) {
	//		registry.remove_all_components_of(entity);
	//	}
	//}

	for (int i = registry.healths.size() - 1; i >= 0; i--) {
		Entity e = registry.healths.entities[i];
		if (registry.healths.components[i].health <= 0) {
			if (registry.players.has(e)) {
				end_screen = true;
				canMove = false;
				createEndScreen(renderer,motion.position, false, 3);
			}
			else {
				registry.remove_all_components_of(e);
			}
		}
	}

	// Processing the salmon state
	//assert(registry.screenStates.components.size() <= 1);
	//ScreenState &screen = registry.screenStates.components[0];

	//float min_counter_ms = 3000.f;
	//for (Entity entity : registry.deathTimers.entities)
	//{
	//	// progress timer
	//	DeathTimer &counter = registry.deathTimers.get(entity);
	//	counter.counter_ms -= elapsed_ms_since_last_update;
	//	if (counter.counter_ms < min_counter_ms)
	//	{
	//		min_counter_ms = counter.counter_ms;
	//	}

	//	// restart the game once the death timer expired
	//	if (counter.counter_ms < 0)
	//	{
	//		registry.deathTimers.remove(entity);
	//		//screen.darken_screen_factor = 0;
	//		restart_game();
	//		return true;
	//	}
	//}
	// reduce window brightness if any of the present salmons is dying
	//screen.darken_screen_factor = 1 - min_counter_ms / 3000;

	// !!! TODO A1: update LightUp timers and remove if time drops below zero, similar to the death counter
	float time = elapsed_ms_since_last_update / 1000.f;

	for (int i = registry.particleSources.entities.size() - 1; i >= 0; i--) {
		ParticleSource& ps = registry.particleSources.components[i];
		ps.life_span -= time;
		if (ps.life_span <= 0) {
			registry.remove_all_components_of(registry.particleSources.entities[i]);
			continue;
		}
		for (int j = 0; j < ps.size; j++) {
			ps.positions[j] += ps.velocities[j] * time;
		}
	}

	wall_timer = std::max(-1.f, wall_timer - elapsed_ms_since_last_update);
	push_timer = std::max(-1.f, push_timer - elapsed_ms_since_last_update);

	// generate wall when drawing is done or exceeds length limit
	if (wall_hinges.size() > 5 || right_mouse_down == false) {
		// at least 2 nodes
		if (wall_hinges.size() < 2) {
			wall_hinges.clear();
		}
		else {
			for (int i = registry.customMeshes.size() - 1; i >= 0; i--) {
				registry.remove_all_components_of(registry.customMeshes.entities[i]);
			}
			createNonConvexWall(20.f, wall_hinges);
			wall_hinges.clear();
			wall_timer = WALL_COOLDOWN;
		}
	}

	for (int i = 0; i < registry.items.size(); i++) {
		Item& item = registry.items.components[i];
		if (!item.active) {
			item.respawn_timer -= elapsed_ms_since_last_update;
			if (item.respawn_timer <= 0) {
				item.active = true;
				item.respawn_timer = ITEM_RESPAWN_DELAY;
				registry.itemColliders.emplace(registry.items.entities[i], 50.f);
			}
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
	left_mouse_down = false;
	plant_timer=PLANT_TIMER_MS;
	explode_timer=BOMB_TIMER_MS;
	defuse_timer=DEFUSE_TIMER_MS;
	bomb_planted=false;
	is_planting=false;
	is_defusing=false;
	win_game=false;
    canMove = true;
	end_screen = false;


	Mix_HaltChannel(-1);
	Mix_Volume(-1,MIX_MAX_VOLUME/20);

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all fish, turtles, ... but that would be more cumbersome
	registry.clear_all_components();

	// Debugging for memory/component leaks
	registry.list_all_components();
	// create ground
	createGround(renderer, current_map);
	
	SetupMap(renderer, current_map);
	createMatrix("src/map/map" + to_string(current_map) + ".json");
	createUI(renderer, attack_mode);
  
	std::vector<vec3> vertices = {
		vec3(-1.f, 1.f, 0.f),
		vec3(1.f, 1.f, 0.f),
		vec3(1.f, -1.f, 0.f),
		vec3(-1.f, -1.f, 0.f),
	};
	std::vector<unsigned int> indices = { 0, 1, 3, 1, 3, 2 };
	createLightSource(vec2(0, 0), vertices, indices);

	
	string src = PROJECT_SOURCE_DIR;
	src += "src/map/map" + to_string(current_map) + ".json";
	ifstream ifs(src);
	
	ifs >> j;

	if (attack_mode){
		// AI defending site
		std::set<int> guard_pos;
		for (int i = 0; i < MAX_TURTLES; i++){
			entity = createTurtle(renderer, vec2{200.f * (float)i + (float)j["defense_spawn"]["x"] - 500, (float)j["defense_spawn"]["y"]});
			Motion &motion = registry.motions.get(entity);
			Enemy &enemy = registry.enemies.get(entity);
			if (i >=0 && i < 4){
				int r = rand() % j["AI_guard_pos"]["defense"].size();
				while (guard_pos.find(r) != guard_pos.end()){
					r += 1;
					if (r == j["AI_guard_pos"]["defense"].size()){
						r = 0;
					}
				} 
				guard_pos.insert(r);
				vec2 pos = {(float)j["AI_guard_pos"]["defense"][r]["x"], (float)j["AI_guard_pos"]["defense"][r]["y"]};
				enemy.pos = pos;
				enemy.guard_mode = true;
			}
		}
		// Create a new salmon
		player_salmon = createSalmon(renderer, { (float)j["attack_spawn"]["x"], (float)j["attack_spawn"]["y"]});
		registry.colors.insert(player_salmon, {1, 0.8f, 0.8f});
	
	} else { 
		 //AI attcking random site
		std::set<int> guard_pos;
		srand((unsigned int) time(NULL));
		int side = rand() % 2;
		attack_side = side;
		for (int i = 0; i < MAX_TURTLES; i++){
			Entity entity = createTurtle(renderer, {100.f * i + 2000.f, 4800.f});
			Motion &motion = registry.motions.get(entity);
			Enemy &enemy = registry.enemies.get(entity);
			int r = rand() % j["AI_guard_pos"]["attack"][side].size();
			while (guard_pos.find(r) != guard_pos.end()){
				r += 1;
				if (r == j["AI_guard_pos"]["attack"][side].size()){
					r = 0;
				}
			} 
			guard_pos.insert(r);
			vec2 pos = {j["AI_guard_pos"]["attack"][side][r]["x"], j["AI_guard_pos"]["attack"][side][r]["y"]};
			enemy.pos = pos;
			enemy.guard_mode = true;

		}

		//// Create a new salmon
		player_salmon = createSalmon(renderer, {j["defense_spawn"]["x"], j["defense_spawn"]["y"]});
		registry.colors.insert(player_salmon, {1, 0.8f, 0.8f});
	}

	// CLEAN

	//createWall(renderer, { 300, 100 }, 0.f, { 200, 200 });
	//createWall(renderer, { 900, 100 }, 0.f, { 200, 200 });
	//createWall(renderer, { 300, 500 }, 0.f, { 200, 200 });
	//createWall(renderer, { 700, 500 }, 0.f, { 200, 200 });
	//createWall(renderer, { 1100, 500 }, 0.f, { 200, 200 });
	//createWall(renderer, { 1100, 700 }, 0.f, { 200, 200 });
    // create story box
    if(toggle[0] == -1) {
        boxes[0] = createStoryBox(renderer, BOX1_LOCATION);
    }

    if(toggle[1] == -1) {
        boxes[1] = createStoryBox(renderer, BOX2_LOCATION);
    }

    if(toggle[2] == -1) {
        boxes[2] = createStoryBox(renderer, BOX3_LOCATION);
    }

    if(toggle[3] == -1) {
        boxes[3] = createStoryBox(renderer, BOX4_LOCATION);
    }
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

    if (action == GLFW_PRESS && key == GLFW_KEY_SPACE && helpMenu.showTutorial1 && toggle[0] == 0)
    {
        helpMenu.showTutorial1 = false;
        canMove = true;
        registry.remove_all_components_of(boxes[0]);
    } else if (action == GLFW_PRESS && key == GLFW_KEY_SPACE && helpMenu.showTutorial2 && toggle[1] == 0)
    {
        helpMenu.showTutorial2 = false;
        canMove = true;
        registry.remove_all_components_of(boxes[1]);
    } else if (action == GLFW_PRESS && key == GLFW_KEY_SPACE && helpMenu.showTutorial3 && toggle[2] == 0)
    {
        helpMenu.showTutorial3 = false;
        canMove = true;
        registry.remove_all_components_of(boxes[2]);
    } else if (action == GLFW_PRESS && key == GLFW_KEY_SPACE && helpMenu.showTutorial4 && toggle[3] == 0)
    {
        helpMenu.showTutorial4 = false;
        canMove = true;
        registry.remove_all_components_of(boxes[3]);
    }

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		restart_game();
	}

	if (win_game && key == GLFW_KEY_ENTER){
		if (!attack_mode){
			current_map+=1;
			if (current_map > 3){
				current_map = 1;
			}
		}
		attack_mode = !attack_mode;

		restart_game();
	}

	if(end_screen){
		input.up = 0;
		input.down = 0;
		input.left = 0;
		input.right = 0;
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
			
			if (!bomb_planted && attack_mode){
				if (action == GLFW_PRESS) {
					if (can_plant ) {
						is_planting = true;
						cout << "planting";
						Mix_PlayChannel(-1, bomb_planting_sound, 0);
					} 

				}
				else if (action == GLFW_RELEASE) {
					plant_timer = PLANT_TIMER_MS;
					is_planting = false;
					//cout << "plant release";
				}
			}
			if (!attack_mode && bomb_planted){
				if (action == GLFW_PRESS) {
					vec2 bomb_pos = registry.motions.get(registry.bomb.entities[0]).position;
					vec2 player_pos = registry.motions.get(player_salmon).position;
					
        			int distance = sqrt(pow(bomb_pos.x - player_pos.x, 2) + pow(bomb_pos.y - player_pos.y, 2));
					if (distance < 50) {
						is_defusing = true;
						cout << "defusing";
						Mix_PlayChannel(-1, defuse_sound, 0);
					} 

				}
				else if (action == GLFW_RELEASE) {
					defuse_timer = DEFUSE_TIMER_MS;
					is_defusing = false;
					cout << "defuse release";
				}
			}
		}
		
	}
	
	
	//if (key == GLFW_KEY_Q)
	//{
	//	if (action == GLFW_PRESS)
	//	{
	//		if (maxWall > 0){
	//			cout << buildmode;
	//			buildmode = !buildmode;
	//		}
	//	}
	//}

	// Player movment WASD
	if (!registry.deathTimers.has(player_salmon) && canMove)
	{
		if (action == GLFW_PRESS)
		{
			if (key == GLFW_KEY_W)
			{
				input.up = 1.f;
			}
			else if (key == GLFW_KEY_S)
			{
				input.down = 1.f;
			}
			else if (key == GLFW_KEY_A)
			{
				input.left = 1.f;
			}
			else if (key == GLFW_KEY_D)
			{
				input.right = 1.f;
			}
		}
		if (action == GLFW_RELEASE)
		{
			if (key == GLFW_KEY_W)
			{
				input.up = 0;
			}
			else if (key == GLFW_KEY_S)
			{
				input.down = 0;
			}
			else if (key == GLFW_KEY_A)
			{
				input.left = 0;
			}
			else if (key == GLFW_KEY_D)
			{
				input.right = 0;
			}
		}

		if ((is_planting && attack_mode) || (is_defusing && !attack_mode)) {
			input.up = 0;
			input.down = 0;
			input.left = 0;
			input.right = 0;
		}
	}

	if (key == GLFW_KEY_Q && action == GLFW_PRESS && push_timer <= 0.f) {
		// create an area that apply impulse
		push_timer = PUSH_COOLDOWN;
		Motion& player_motion = registry.motions.get(player_salmon);
		createPushArea(player_motion.position, 500.f, player_motion.angle, M_PI / 3.f);
		createParticleSource(100, 2.f, 0.2f, vec3(0.7f, 0.7f, 0.7f), player_motion.position, player_motion.angle, 1500.f);
	}


	// Control the current speed with `<` `>`
	//if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA)
	//{
	//	current_speed -= 0.1f;
	//	printf("Current speed = %f\n", current_speed);
	//}
	//if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD)
	//{
	//	current_speed += 0.1f;
	//	printf("Current speed = %f\n", current_speed);
	//}
	//current_speed = fmax(0.f, current_speed);
}

void WorldSystem::on_mouse_move(vec2 mouse_position)
{

	Motion &motion = registry.motions.get(player_salmon);

	int w, h;
	glfwGetWindowSize(window, &w, &h);
	vec2 relative_position = vec2(mouse_position.x - w / 2.f, mouse_position.y - h / 2.f);

	float angle = atan2(relative_position.y, relative_position.x);

	motion.angle = angle;
	mousecoord = mouse_position;

	if (right_mouse_down && wall_timer <= 0.f) {
		vec2 mouse_world_coord = vec2(relative_position.x / (w / 2.f) * (window_width_px / 2.f)
			, relative_position.y / (h / 2.f) * (window_height_px / 2.f)) + motion.position;
		if (wall_hinges.size() == 0 || length(mouse_world_coord - wall_hinges.back()) > 100.f) {
			wall_hinges.push_back(mouse_world_coord);
		}
	}
}

void WorldSystem::on_mouse_click(int button, int action, int mods)
{
	if (buildmode){
		if (button == GLFW_MOUSE_BUTTON_1) {
			if (action == GLFW_PRESS)
			{
				left_mouse_down = true;
			}
			else if (action == GLFW_RELEASE)
			{
				left_mouse_down = false;
			}
		}
	}else {
		if (button == GLFW_MOUSE_BUTTON_1) {
			if (action == GLFW_PRESS)
			{
				left_mouse_down = true;
				tap = true;
			}
			else if (action == GLFW_RELEASE)
			{
				left_mouse_down = false;
			}
		}
	}

	if (button == GLFW_MOUSE_BUTTON_2) {
		if (action == GLFW_PRESS)
		{
			right_mouse_down = true;
		}
		else if (action == GLFW_RELEASE)
		{
			right_mouse_down = false;
		}
	}
}

// e1 should be the bullet
void WorldSystem::handle_bullet_hit(Entity bullet, Entity entity) {
	Health& health = registry.healths.get(entity);
	if (registry.healths.has(entity)) {
		health.health -= 20;
	}

	assert(registry.motions.has(bullet));

	Motion& bullet_motion = registry.motions.get(bullet);

	if (registry.nonConvexWallColliders.has(entity)) {
		vec2 dir = -normalize(bullet_motion.velocity);
		createParticleSource(50, 2.f, 1.f, vec3(0.f, 0.f, 0.f), bullet_motion.position, atan2(dir.y, dir.x), 150.f);
	}
	else if (registry.enemies.has(entity) || registry.players.has(entity)) {
		vec2 dir = -normalize(bullet_motion.velocity);
		createParticleSource(50, 2.f, 1.f, vec3(1.f, 0.f, 0.f), bullet_motion.position, atan2(dir.y, dir.x), 150.f);
	}
}

void WorldSystem::update_player_velocity() {
	float multiplier = 1.f;
	if (registry.boosts.has(player_salmon)) {
		multiplier = registry.boosts.get(player_salmon).speed_multiplier;
	}
	registry.motions.get(player_salmon).velocity = player_speed * multiplier * vec2(input.right - input.left, input.down - input.up);
}

void WorldSystem::handle_items(Entity entity_1, ITEM_TYPE type) {
	if (type == ITEM_TYPE::HEALTH_REGEN) {
		Health& health = registry.healths.get(entity_1);
		health.health = std::min(health.health + 500.f, 1000.f);
	}
	else if (type == ITEM_TYPE::SPEED_BOOST) {
		registry.boosts.emplace(entity_1);
	}
}
  
int WorldSystem::getCurrentMap() {
    return current_map;
}
