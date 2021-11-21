#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"
#define ROW 50
#define COL 50
using MyArray = std::array<std::array<int, ROW>, COL>;

// These are ahrd coded to the dimensions of the entity texture
const float FISH_BB_WIDTH = 0.4f * 100.f;
const float FISH_BB_HEIGHT = 0.4f * 100.f;
const float TURTLE_BB_WIDTH = 0.4f * 300.f;
const float TURTLE_BB_HEIGHT = 0.4f * 202.f;

// the player
Entity createSalmon(RenderSystem *renderer, vec2 pos);
Entity createWall(RenderSystem *renderer, vec2 pos, float angle, vec2 scale);
// the prey
Entity createStoryBox(RenderSystem *renderer, vec2 position);
// the enemy
Entity createTurtle(RenderSystem *renderer, vec2 position);
// a red line for debugging purposes
Entity createLine(vec2 position, float angle, vec2 size);
// a pebble
Entity createPebble(vec2 pos, vec2 size);
MyArray createMatrix();
int createGround(RenderSystem *renderer);
int SetupMap(RenderSystem *renderer);

void Fill(MyArray &T);
void Print(const MyArray &T);

Entity createBullet(RenderSystem *renderer, vec2 position, float angle);
Entity createShockwave(vec2 pos);
