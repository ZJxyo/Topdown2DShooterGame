//
// Created by Ivy He on 2021-10-30.
//

#include "HelpMenu.h"
#include "tiny_ecs_registry.hpp"
#include <chrono>
#include <thread>


Entity HelpMenu::createMenu(RenderSystem* renderer, GLFWwindow* window,vec2 position) {
    // Reserve en entity
    auto entity = Entity();
    HelpMenu::showMenu = true;

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Initialize the position, scale, and physics components
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.position = position;

    motion.scale = vec2({ MENU_WIDTH, MENU_HEIGHT });

    if (glfwGetKey(window, GLFW_KEY_1)) {
        registry.renderRequests.insert(
                entity,
                { TEXTURE_ASSET_ID::HELP1,
                  EFFECT_ASSET_ID::TEXTURED,
                  GEOMETRY_BUFFER_ID::SPRITE });

    } else if (glfwGetKey(window, GLFW_KEY_2)) {
        registry.renderRequests.insert(
                entity,
                { TEXTURE_ASSET_ID::HELP2,
                  EFFECT_ASSET_ID::TEXTURED,
                  GEOMETRY_BUFFER_ID::SPRITE });
    } else if (glfwGetKey(window, GLFW_KEY_3)) {
        registry.renderRequests.insert(
                entity,
                { TEXTURE_ASSET_ID::HELP3,
                  EFFECT_ASSET_ID::TEXTURED,
                  GEOMETRY_BUFFER_ID::SPRITE });
    } else if( glfwGetKey(window, GLFW_KEY_ENTER)) {
        showMenu = false;
//        showStory1 = true;
        registry.remove_all_components_of(entity);
//        printf("cleared");
//        renderer->draw();
    }
    return entity;
}

Entity HelpMenu::createInto(RenderSystem *renderer, GLFWwindow *window, vec2 position) {
//    printf("enter show intro");
    // Reserve en entity
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Initialize the position, scale, and physics components
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.position = position;

    motion.scale = vec2({ MENU_WIDTH, MENU_HEIGHT });
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::HELP0,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });
//    std::this_thread::sleep_for(std::chrono::milliseconds(10));
//    registry.remove_all_components_of(entity2);
//    HelpMenu::showInto = false;
    return entity;
}

Entity HelpMenu::createStroy1(RenderSystem *renderer, GLFWwindow *window, vec2 position) {
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Initialize the position, scale, and physics components
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.position = position;

    motion.scale = vec2({ MENU_WIDTH, MENU_HEIGHT });
    registry.storyBox.emplace(entity);
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::STORY1,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });
    return entity;
}

Entity HelpMenu::createStroy2(RenderSystem *renderer, GLFWwindow *window, vec2 position) {
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Initialize the position, scale, and physics components
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.position = position;

    motion.scale = vec2({ MENU_WIDTH, MENU_HEIGHT });
    registry.storyBox.emplace(entity);
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::STORY2,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });
    return entity;
}

Entity HelpMenu::createStroy3(RenderSystem *renderer, GLFWwindow *window, vec2 position) {
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Initialize the position, scale, and physics components
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.position = position;

    motion.scale = vec2({ MENU_WIDTH, MENU_HEIGHT });
    registry.storyBox.emplace(entity);
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::STORY3,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });
    return entity;
}

Entity HelpMenu::createStroy4(RenderSystem *renderer, GLFWwindow *window, vec2 position) {
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Initialize the position, scale, and physics components
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.position = position;

    motion.scale = vec2({ MENU_WIDTH, MENU_HEIGHT });
    registry.storyBox.emplace(entity);
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::STORY4,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });
    return entity;
}