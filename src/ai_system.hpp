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
    BTIfCondition(BTNode* move, BTNode* shoot, BTNode* build) : move(move) , shoot(shoot), build(build){

    }

    virtual void init(Entity e) override {
        move->init(e);
    }

    virtual BTState process(Entity e) override {
        if (registry.motions.has(e)) {


            if (move->process(e) == BTState::Success) {
                return BTState::Success;
            }
            else if(shoot->process(e) == BTState::Success){
                return BTState::Success;
            }
            else if (build->process(e) == BTState::Success) {
                return BTState::Success;
            }

            return BTState::Success;
        }
        else {
            return BTState::Success;
        }

    }
private:
    BTNode* move;
    BTNode* shoot;
    BTNode* build;
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
        int distance = sqrt(pow(playerX - AIX, 2) + pow(playerY - AIY, 2));

        if (distance<225) {
            return BTState::Failure;
        }

        return BTState::Success;
    }
};

class Shoot : public BTNode {
public:
    Shoot(Entity other) {
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

        
        
        //        vel += 20;
        printf("Turn to the player and shoot 5 bullets\n");
        for (int i = 0;i < 5;i++) {
            printf("Bullet #%d\n", i);
            registry.motions.get(e).angle = atan2(playerX - registry.motions.get(e).position.x, playerY - registry.motions.get(e).position.y);
        }
        return BTState::Failure; // return failure if shoot five time, success otherwirse. // naive one
    }
};

class Build : public BTNode {
public:
    Build(Entity other) {
        player = other;
    }
private:
    Entity player;
    void init(Entity e) override {}

    BTState process(Entity e) override {
       
        printf("After shooting, start to build a wall\n");

        return BTState::Success;
    }
};