// internal
#include "ai_system.hpp"
#include <queue>


void AISystem::step(float elapsed_ms)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE FISH AI HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// You will likely want to write new functions and need to create
	// new data structures to implement a more sophisticated Fish AI.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	(void)elapsed_ms; // placeholder to silence unused warning until implemented

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: DRAW DEBUG INFO HERE on AI path
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// You will want to use the createLine from world_init.hpp
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}


// Direction vectors
int dRow[] = { -1, 0, 1, 0, 1, 1, -1, -1};
int dCol[] = { 0, -1, 0, 1 ,1, -1, 1, -1};


bool AISystem::isValid(int row, int col) {
    // If cell lies out of bounds
    if (row < 0 || col < 0 || row >= ROW || col >= COL || grid[row][col] == 1)
        return false;

    // If cell is already visited
    if (vis[row][col])
        return false;

    // Otherwise
    return true;
}

void AISystem::BFS(int startRow, int startCol, int endRow, int endCol) {
    // Stores indices of the matrix cells
    queue<pair<int, int> > q;

    // Mark the starting cell as visited
    // and push it into the queue
    q.push({ startRow, startCol });
    vis[startRow][startCol] = true;

    // Iterate while the queue
    // is not empty
    while (!q.empty()) {
//        std::cout <<"queue size: " << q.size() << " \n";


        pair<int, int> cell = q.front();
        int x = cell.first;
        int y = cell.second;
        if(x == endRow && y == endCol) {
//            std::cout <<"reached the goal" << " \n";
//            std::cout << "total path length " << path.size() << " \n";
            for(pair<int, int> i: path) {
//                std::cout << "path x : " << i.first << " path y : " << i.second << " \n";
            }
            return;
        }

//        std::cout << "row : " << x << " col : " << y << " \n";

        path.push_back(q.front());
        q.pop();

        // Go to the adjacent cells
        for (int i = 0; i < 4; i++) {

            int adjx = x + dRow[i];
            int adjy = y + dCol[i];

            if (isValid(adjx, adjy)) {
                q.push({ adjx, adjy });
                vis[adjx][adjy] = true;
            }
        }
    }
}
