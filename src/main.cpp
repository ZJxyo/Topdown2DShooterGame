
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
Entity stories[5];
int toggle = 1;

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
        if(helpMenu.showInto && toggle == 1) {
            stories[0] = helpMenu.createStory1(&renderer, window, INTRO_LOCATION);
            toggle = 2;

        } else if(helpMenu.showInto && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && toggle == 2) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            registry.remove_all_components_of(stories[0]);
            stories[1] = helpMenu.createStory2(&renderer, window, INTRO_LOCATION);
            toggle = 3;

        } else if(helpMenu.showInto && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && toggle == 3) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            registry.remove_all_components_of(stories[1]);
            stories[2] = helpMenu.createStory3(&renderer, window, INTRO_LOCATION);
            toggle = 4;

        } else if(helpMenu.showInto && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && toggle == 4) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            registry.remove_all_components_of(stories[2]);
            stories[3] = helpMenu.createStory4(&renderer, window, INTRO_LOCATION);
            toggle = 5;

        } else if(helpMenu.showInto && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && toggle == 5) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            registry.remove_all_components_of(stories[3]);
            stories[4] = helpMenu.createStory5(&renderer, window, INTRO_LOCATION);
            toggle = 6;

        } else if(helpMenu.showInto && glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && toggle == 6) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            registry.remove_all_components_of(stories[4]);
            helpMenu.showInto = false;
        } else {
            world.step(elapsed_ms);
            ai.step(elapsed_ms);
            physics.step(elapsed_ms);
            //world.handle_collisions();
        }


		// TODO A2: you can implement the debug freeze here but other places are possible too.
	}

	return EXIT_SUCCESS;
}
