
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "ai_system.hpp"
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"
#include "HelpMenu.h"

using Clock = std::chrono::high_resolution_clock;
const vec2 INTRO_LOCATION = {1000, 4800};


// Entry point
int main()
{
	// Global systems
	WorldSystem world;
	RenderSystem renderer;
	PhysicsSystem physics;
	AISystem ai;
    HelpMenu helpMenu;


	// Initializing window
	GLFWwindow* window = world.create_window();

	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}

	// initialize the main systems
	renderer.init(window_width_px, window_height_px, window);
	world.init(&renderer);

	physics.bullet_hit_callbacks.emplace_back(WorldSystem::handle_bullet_hit);

	// variable timestep loop
	auto t = Clock::now();
	while (!world.is_over()) {
		// Processes system messages, if this wasn't present the window would become
		// unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

        renderer.draw();
        // menu intro loop
        if(helpMenu.showInto) {
            helpMenu.createInto(&renderer, window, INTRO_LOCATION);
//            ai.BFS(0,0, 40,35);
            helpMenu.showInto = false;
        }

        // show menu page loop
        if(helpMenu.showMenu && !helpMenu.showInto) {
            helpMenu.createMenu(&renderer, window, INTRO_LOCATION);
            if(!helpMenu.showMenu) {
                world.init(&renderer);
            }
        }
        // game loop
        else {
            world.step(elapsed_ms);
            ai.step(elapsed_ms);
            physics.step(elapsed_ms);
            //world.handle_collisions();
        }


		// TODO A2: you can implement the debug freeze here but other places are possible too.
	}

	return EXIT_SUCCESS;
}
