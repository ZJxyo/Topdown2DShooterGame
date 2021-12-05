#include "HelpMenu.h"
#include "tiny_ecs_registry.hpp"
#include <chrono>
#include <thread>

Entity HelpMenu::createStory1(RenderSystem *renderer, GLFWwindow *window, vec2 position) {
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
            { TEXTURE_ASSET_ID::STORY5,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });
    return entity;
}

Entity HelpMenu::createStory2(RenderSystem *renderer, GLFWwindow *window, vec2 position) {
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
            { TEXTURE_ASSET_ID::STORY6,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });
    return entity;
}

Entity HelpMenu::createStory3(RenderSystem *renderer, GLFWwindow *window, vec2 position) {
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
            { TEXTURE_ASSET_ID::STORY7,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });
    return entity;
}

Entity HelpMenu::createStory4(RenderSystem *renderer, GLFWwindow *window, vec2 position) {
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
            { TEXTURE_ASSET_ID::STORY8,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });
    return entity;
}

Entity HelpMenu::createStory5(RenderSystem *renderer, GLFWwindow *window, vec2 position) {
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
            { TEXTURE_ASSET_ID::STORY9,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });
    return entity;
}

Entity HelpMenu::createTutorial1(RenderSystem *renderer, GLFWwindow *window, vec2 position) {
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

Entity HelpMenu::createTutorial2(RenderSystem *renderer, GLFWwindow *window, vec2 position) {
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

Entity HelpMenu::createTutorial3(RenderSystem *renderer, GLFWwindow *window, vec2 position) {
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

Entity HelpMenu::createTutorial4(RenderSystem *renderer, GLFWwindow *window, vec2 position) {
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