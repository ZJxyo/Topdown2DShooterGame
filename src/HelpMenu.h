//
// Created by Ivy He on 2021-10-30.
//

#ifndef SALMON_HELPMENU_H
#define SALMON_HELPMENU_H


#include "tiny_ecs.hpp"
#include "render_system.hpp"
static const float MENU_WIDTH = 2.5f * 296.f;
static const float MENU_HEIGHT = 2.5f * 165.f;

class HelpMenu {
public:
    Entity createInto(RenderSystem* renderer, GLFWwindow* window, vec2 position);
    Entity createMenu(RenderSystem* renderer, GLFWwindow* window, vec2 position);
    Entity createStroy1(RenderSystem* renderer, GLFWwindow* window, vec2 position);
    Entity createStroy2(RenderSystem* renderer, GLFWwindow* window, vec2 position);
    Entity createStroy3(RenderSystem* renderer, GLFWwindow* window, vec2 position);
    Entity createStroy4(RenderSystem* renderer, GLFWwindow* window, vec2 position);
    bool showMenu = true;
    bool showInto = true;
    bool showStory1 = true;
    bool showStory2 = true;
    bool showStory3 = true;
    bool showStory4 = true;
private:
    static RenderSystem* renderer;
    // OpenGL window handle
    static GLFWwindow* window;
};


#endif //SALMON_HELPMENU_H
