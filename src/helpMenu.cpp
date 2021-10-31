//
// Created by Ivy He on 2021-10-30.
//

#include "helpMenu.h"
#include "tiny_ecs_registry.hpp"

Entity helpMenu::createMenu(RenderSystem *renderer, vec2 position) {
    // Reserve en entity
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::HELP0);
    registry.meshPtrs.emplace(entity, &mesh);

    // Initialize the position, scale, and physics components
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { -50, 0 };
    motion.position = position;

    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = vec2({ -MENU_WIDTH, MENU_HEIGHT });

    // Create an (empty) Fish component to be able to refer to all fish
    registry.softShells.emplace(entity);
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::HELP0,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::HELP0 });


    return entity;
}
