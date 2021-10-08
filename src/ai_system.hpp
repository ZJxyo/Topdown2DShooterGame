#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "world_system.hpp"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 3
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

class AISystem
{
public:
    void step(float elapsed_ms);
};

enum class BTState {
    Running,
    Success,
    Failure
};

class BTNode {
public:
    virtual void init(Entity e) {};

    virtual BTState process(Entity e) = 0;
};

class BTIfCondition : public BTNode {
public:
    BTIfCondition(BTNode* child) : m_child(child) {

    }

    virtual void init(Entity e) override {
        m_child->init(e);
    }

    virtual BTState process(Entity e) override {
        if (registry.motions.has(e))
            return m_child->process(e);
        else
            return BTState::Success;
    }
private:
    BTNode* m_child;
};

class Move : public BTNode {
public:
    Move(Entity other) {
        player = other;
    }
private:
    Entity player;
    void init(Entity e) override {}

    BTState process(Entity e) override {
        auto& vel = registry.motions.get(e).velocity;
        //WorldSystem::getEntity()
        int playerX = registry.motions.get(player).position.x;
        int playerY = registry.motions.get(player).position.y;
        int AIX = registry.motions.get(e).position.x;
        int AIY = registry.motions.get(e).position.y;


        if (AIX > playerX) {

            vel.x = -100;
        }
        if (AIX < playerX) {
            vel.x = 100;
        }

        if (AIY > playerY) {
            vel.y = -100;
        }
        if (AIY < playerY) {
            vel.y = +100;
        }
        //        vel += 20;

        // if distance not within 5m, return success, else return failure
        return BTState::Success;
    }
};