//
// Created by Ivy He on 2021-10-30.
//

#ifndef SALMON_HELPMENU_H
#define SALMON_HELPMENU_H


#include "tiny_ecs.hpp"
#include "render_system.hpp"
static const float MENU_WIDTH = 1000;
static const float MENU_HEIGHT = 650;

class HelpMenu {
public:
    Entity createStory1(RenderSystem* renderer, GLFWwindow* window, vec2 position);
    Entity createStory2(RenderSystem* renderer, GLFWwindow* window, vec2 position);
    Entity createStory3(RenderSystem* renderer, GLFWwindow* window, vec2 position);
    Entity createStory4(RenderSystem* renderer, GLFWwindow* window, vec2 position);
    Entity createStory5(RenderSystem* renderer, GLFWwindow* window, vec2 position);
    Entity createTutorial1(RenderSystem* renderer, GLFWwindow* window, vec2 position);
    Entity createTutorial2(RenderSystem* renderer, GLFWwindow* window, vec2 position);
    Entity createTutorial3(RenderSystem* renderer, GLFWwindow* window, vec2 position);
    Entity createTutorial4(RenderSystem* renderer, GLFWwindow* window, vec2 position);
    bool showInto = true;
    bool showTutorial1 = true;
    bool showTutorial2 = true;
    bool showTutorial3 = true;
    bool showTutorial4 = true;
private:
    static RenderSystem* renderer;
    // OpenGL window handle
    static GLFWwindow* window;
};


#endif //SALMON_HELPMENU_H
