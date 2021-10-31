//
// Created by Ivy He on 2021-10-30.
//

#ifndef SALMON_HELPMENU_H
#define SALMON_HELPMENU_H


#include "tiny_ecs.hpp"
#include "render_system.hpp"
static const float MENU_WIDTH = 0.4f * 296.f;
static const float MENU_HEIGHT = 0.4f * 165.f;

class helpMenu {
public:
    static Entity createMenu(RenderSystem* renderer, vec2 position);
};


#endif //SALMON_HELPMENU_H
