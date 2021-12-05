#pragma once

#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <math.h>
#include <stack>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "world_system.hpp"
#include "world_init.hpp"
using namespace std;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 3
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define ROW 50
#define COL 50

const size_t BULLET_TIMER_AI_MS = 100;

struct Node
{
    int y;
    int x;
    int parentX;
    int parentY;
    float gCost;
    float hCost;
    float fCost;
};


class AISystem
{
public:
    void step(float elapsed_ms);
    // Cite from: https://www.geeksforgeeks.org/breadth-first-traversal-bfs-on-a-2d-array/
    bool isValid(int row, int col);
    void BFS(int startRow, int startCol, int endRow, int endCol);
    stack<pair<int, int>> findPath(int startRow, int startCol, int endRow, int endCol);
    AISystem() {
        memset(vis, false, sizeof vis);
    }
    stack<pair<int, int>> path;
    pair<int, int> route[ROW][COL];

    //bool isValid(int row, int col);
    bool isDestination(int x, int y, Node dest);
    double calculateH(int x, int y, Node dest);
    vector<Node> makePath(array<array<Node, (50)>, (50)> map, Node dest);
    vector<Node> aStar(Node player, Node dest);
private:
    bool vis[ROW][COL];
    //    int grid[ROW][COL] = { { 1, 2, 3, 4 },
    //                           { 5, 6, 7, 8 },
    //                           { 9, 10, 11, 12 },
    //                           { 13, 14, 15, 16 } };
    int grid[ROW][COL] = { {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
                              {1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
                              {1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
                              {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
                              {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
                              {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
                              {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
                              {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
                              {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
                              {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
                              {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                              {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                              {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                              {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                              {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                              {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                              {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
                              {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                              {1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                              {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                              {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                              {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                              {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                              {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                              {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                              {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                              {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                              {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                              {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} };
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

// A composite node that loops through all children and exits when one fails
class BTRunPair : public BTNode {
private:
    int m_index;
    BTNode* m_children[2];

public:
    BTRunPair(BTNode* c0, BTNode* c1)
        : m_index(0) {
        m_children[0] = c0;
        m_children[1] = c1;
    }

    void init(Entity e) override
    {
        m_index = 0;
        // initialize the first child
        const auto& child = m_children[m_index];
        child->init(e);
    }

    BTState process(Entity e) override {
        if (m_index >= 2)
            return BTState::Success;

        // process current child
        BTState state = m_children[m_index]->process(e);

        // select a new active child and initialize its internal state
        if (state == BTState::Success) {
            ++m_index;
            if (m_index >= 2) {
                return BTState::Success;
            }
            else {
                m_children[m_index]->init(e);
                return BTState::Running;
            }
        }
        else {
            return state;
        }
    }
};

class BTIfCondition : public BTNode {
public:
    BTIfCondition(BTNode* chase, BTNode* shoot, BTNode* build) : chase(chase), shoot(shoot), build(build) {
    }

    virtual void init(Entity e) override {
        chase->init(e);
    }

    virtual BTState process(Entity e1) override {
        BTState state = BTState::Success;
        int count = 1;
            for (Entity e : registry.enemies.entities) {
                // std::cout << "count: " << count << std::endl;
                if (registry.motions.has(e)) {


                    if (chase->process(e) == BTState::Success) {
                        //return BTState::Success;
                        state = BTState::Success;
                    }

                    if (chase->process(e) != BTState::Success) {
                        int bombX = 0;
                        int bombY = 0;
                        bool is_planted = false;
                        for (Entity b : registry.bombInfo.entities) {
                            bombX = registry.bombInfo.get(b).position.x;
                            bombY = registry.bombInfo.get(b).position.y;
                            // is_planted = registry.bombInfo.get(b).isPlanted;
                            is_planted = true;

                        }

                        int bombDistance = 10000;
                        Entity closet = e1; //dummy entity
                        if (is_planted) {
                            for (Entity enemy : registry.enemies.entities) {
                                int enemyX = registry.motions.get(enemy).position.x;
                                int enemyY = registry.motions.get(enemy).position.y;
                                int dis = sqrt(pow(enemyX - bombX, 2) + pow(enemyY - bombY, 2));
                                if (dis < bombDistance) {
                                    bombDistance = dis;
                                    closet = enemy;
                                }
                            }

                        }

                        if (e == closet) {
                            // defuse
                        }
                        else {
                            shoot->process(e);
                        }

                        // shoot->process(e) == BTState::Success;
                        //return BTState::Success;
                        state = BTState::Success;
                    }
                    state = BTState::Success;
                    //return BTState::Success;
                }
                else {
                    state = BTState::Success;
                    //return BTState::Success;
                    //std::cout << "don't have" << registry.enemies.entities.size();
                }

                // count++;
            }
        

        return BTState::Success;
    }
private:
    BTNode* chase;
    BTNode* shoot;
    BTNode* build;
};

class Chase : public BTNode {
public:
    Chase(Entity other) {
        player = other;
    }
private:
    Entity player;
    void init(Entity e) override {}

    BTState process(Entity e) override {
        AISystem ai;
        auto& vel = registry.motions.get(e).velocity;
        //WorldSystem::getEntity()
        int playerX = registry.motions.get(player).position.x;
        int playerY = registry.motions.get(player).position.y;
        int AIX = registry.motions.get(e).position.x;
        int AIY = registry.motions.get(e).position.y;
        
        int bombX = 0;
        int bombY = 0;
        bool is_planted = false;
        for (Entity b : registry.bombInfo.entities) {
            
            bombX = registry.bombInfo.get(b).position.x;
            bombY = registry.bombInfo.get(b).position.y;
            // is_planted = registry.bombInfo.get(b).isPlanted;
            is_planted = true;
       
        }

        int bombDistance = 10000;
        Entity closet = player; //dummy entity
        if (is_planted) {
            for (Entity enemy : registry.enemies.entities) {
                int enemyX = registry.motions.get(enemy).position.x;
                int enemyY = registry.motions.get(enemy).position.y;
                int dis = sqrt(pow(enemyX - bombX, 2) + pow(enemyY - bombY, 2));
                if (dis < bombDistance) {
                    bombDistance = dis;
                    closet = enemy;
                }
            }

        }

        // star

        Node player;
        player.y = playerX / 100; // map x, y(express in vector form [8,6]) is equal to y, x (6,8) in coordinate system
        player.x = playerY / 100;

        Node AI;
        AI.y = AIX / 100;
        AI.x = AIY / 100;

        Node bomb;
        bomb.y = bombX / 100;
        bomb.x = bombY / 100;

        std::cout << "bombX: " << bomb.x << std::endl;
        std::cout << "bombY: " << bomb.y << std::endl;

        vector<Node> pathA = ai.aStar(AI, player);
        if (e == closet) {
            pathA = ai.aStar(AI, bomb);
        }

        if (pathA.size() >= 2) {
            Node node = pathA.at(1); // find next place we want to go to 
            if (AIX > node.y * 100 + 50) {

                vel.x = -150;
            }
            if (AIX < node.y * 100 + 50) {

                vel.x = 150;
            }

            if (AIY > node.x * 100 + 50) {

                vel.y = -150;
            }
            if (AIY < node.x * 100 + 50) {

                vel.y = 150;
            }
        }


        // end
        
        // std::cout << "AIX: "<<AIX << std::endl;

        // if bomb is planted, then AI that closet to bomb will chase bomb and defuse it

        //if (e == closet) {
        //    ai.BFS(AIY / 100, AIX / 100, bombY / 100, bombX / 100);
        //}   
        //else {
        //    ai.BFS(AIY / 100, AIX / 100, playerY / 100, playerX / 100);
        //}

        


        //if (!ai.path.empty()) {
        //    pair<int, int> curr = ai.path.top();
        //    if (AIX > curr.second * 100 + 50) {

        //        vel.x = -200;
        //    }
        //    if (AIX < curr.second * 100 + 50 ) {

        //        vel.x = 200;
        //    }

        //    if (AIY > curr.first* 100 + 50) {

        //        vel.y = -200;
        //    }
        //    if (AIY < curr.first * 100+ 50) {

        //        vel.y = +200;
        //    }

        //}

        int distance = sqrt(pow(playerX - AIX, 2) + pow(playerY - AIY, 2));
       // avoid other AI
        int eplison = 150;
        for (Entity enemy : registry.enemies.entities) {

            float y = registry.motions.get(enemy).position.y; //enemy y
            float x = registry.motions.get(enemy).position.x; //enemy x
            auto& velEnemy = registry.motions.get(enemy).velocity; //turtle
            for (Entity enemy1 : registry.enemies.entities) {

                if (enemy == enemy1) { continue; }

                float y1 = registry.motions.get(enemy1).position.y; //enemy y
                float x1 = registry.motions.get(enemy1).position.x; //enemy x

                //check if sourranded by fish
                float rightX = x + eplison;
                float LeftX = x - eplison;
                float downY = y + eplison;
                float upY = y - eplison;

                if (x1<rightX && x1>LeftX && y1< downY && y1>upY) {

                    //yes, avoid it.
                    if (x > x1) {

                        velEnemy.x = 200;
                    }
                    if (x < x1) {
                        velEnemy.x = -200;
                    }

                    if (y > y1) {
                        velEnemy.y = 200;
                    }
                    if (y < y1) {
                        velEnemy.y = -200;
                    }
                }

            }


            //        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //        for (auto it = begin (ai.path); it != end (ai.path); ++it) {
    //            if(vel.x == 0 && vel.y == 0) {
    //                std::cout <<"AI is not moving" << " \n";
    //                vel.x = playerX % 100;
    //                vel.y = playerY % 100;
    //            }
    //            if(it->first*100 >= AIX && it->second*100 <= AIY) {
    ////                std::cout <<"moving upper left" << " \n";
    //                vel.x = 100;
    //                vel.y = -100;
    //            }
    //            if(it->first*100 >= AIX && it->second*100 >= AIY) {
    ////                std::cout <<"moving lower left" << " \n";
    //                vel.x = 100;
    //                vel.y = 100;
    //            }
    //            if(it->first*100 <= AIX && it->second*100 <= AIY) {
    ////                std::cout <<"moving upper right" << " \n";
    //                vel.x = -100;
    //                vel.y = -100;
    //            }
    //            if(it->first*100 <= AIX && it->second*100 >= AIY) {
    ////                std::cout <<"moving lower right" << " \n";
    //                vel.x = -100;
    //                vel.y = 100;
    //            }
    //        }
        }

        if (distance < 200) {
            return BTState::Failure;
        }
        return BTState::Success;
    }
};

class ShootNBullets : public BTNode {
public:
    ShootNBullets(Entity other, RenderSystem* renderer, float elapsed_ms) {
        player = other;
        this->renderer = renderer;
        this->elapsed_ms = elapsed_ms;
    }
private:
    Entity player;
    RenderSystem* renderer;
    float elapsed_ms;
    void init(Entity e) override {}
    BTState process(Entity e) override {
        auto& vel = registry.motions.get(e).velocity;
        //WorldSystem::getEntity()
        int playerX = registry.motions.get(player).position.x;
        int playerY = registry.motions.get(player).position.y;
        Motion& AImotion = registry.motions.get(e);

        if (AImotion.position.x <= playerX) {
            AImotion.angle = atan2(playerY, playerX);
        }
        else {
            AImotion.angle = 3.14 - atan2(playerY, playerX);
        }

        //here to change AI bullet
        float LO = -0.5;
        float HI = 0.5;
        float r3 = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
        FireRate &fr = registry.fireRates.get(e);
        fr.fire_rate -= elapsed_ms;
        if (fr.fire_rate < 0){
            fr.fire_rate = BULLET_TIMER_AI_MS;
            createBullet(renderer, AImotion.position, AImotion.angle + 1.5708 + r3);
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