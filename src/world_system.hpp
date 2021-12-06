#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"
#include "HelpMenu.h"

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();

	// Creates a window

	GLFWwindow* create_window();


	// starts the game
	void init(RenderSystem *renderer);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Check for collisions
	void handle_collisions();
	static void handle_bullet_hit(Entity entity_1, Entity entity_2);

	void update_player_velocity();

	static Entity createParticleSource(uint8 size, float radius, float decay, vec3 color, vec2 pos, vec2 dir, float speed);

	// Should the game be over ?
	bool is_over() const;

private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_click(int button, int action, int mods);

	// restart level
	void restart_game();

	// OpenGL window handle
	GLFWwindow *window;

	// Number of fish eaten by the salmon, displayed in the window title
	unsigned int points;

	// Game state
	RenderSystem *renderer;
	float current_speed;
	float player_speed = 500;
	float next_turtle_spawn;
	float next_fish_spawn;
	float next_animation;
	float next_chase;
	Entity player_salmon;
	bool left_mouse_down;
	bool tap;
    Entity bomb;
	bool can_plant;
	bool bomb_planted;
    bool canMove;
    int toggle[4] = {-1, -1, -1, -1};
    Entity tutorials[4];
    Entity boxes[4];
	float plant_timer;
	float explode_timer;
	bool is_planting;
	bool win_game;
	float footsteps_timer;
	bool buildmode;
	bool building;
	int maxWall;
	bool attack_mode;
	float defuse_timer;
	int attack_side;
	bool is_defusing;
	vec2 buildcoord;
	vec2 mousecoord;
	int current_map;

	// non convex walls
	bool right_mouse_down = false;
	std::vector<vec2> wall_hinges;
	

	// music references
	Mix_Music *background_music;
	Mix_Chunk *salmon_dead_sound;
	Mix_Chunk *salmon_eat_sound;
	Mix_Chunk *bomb_planted_sound;
	Mix_Chunk *bomb_planting_sound;
	Mix_Chunk *bomb_countdown_sound;
	Mix_Chunk *bomb_explosion_sound;
	Mix_Chunk *footsteps_sound;
	Mix_Chunk *ak47_sound;
	Mix_Chunk *defuse_sound;
	

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	struct Input {
		float up = 0;
		float down = 0;
		float left = 0;
		float right = 0;
	};

	Input input;
    HelpMenu helpMenu;
    const vec2 BOX1_LOCATION = {1000, 4800};
    const vec2 BOX2_LOCATION = {1000, 4600};
    const vec2 BOX3_LOCATION = {1000, 4400};
    const vec2 BOX4_LOCATION = {1000, 4200};
};
