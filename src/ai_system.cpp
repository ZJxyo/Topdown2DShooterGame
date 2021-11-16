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

stack<pair<int, int>>
AISystem::findPath(int startRow, int startCol, int endRow, int endCol) {
    if(startRow == endRow && startCol == endCol) return path;
    int x = endRow, y = endCol;
//    std::cout <<" hello" << endl;
    int counter = 5;
    while(route[x][y].first != startRow || route[x][y].second != startCol) {
        std::cout <<" hello" << endl;
//        std::cout <<"inside find path, push back: route x " << route[x][y].first << " rount y :"<< route[x][y].second<< " \n";
        path.push(route[x][y]);
        int temp = x;
        x = route[x][y].first;
        y = route[temp][y].second;
//        if(--counter == 0) break;
    }
    return path;
}

void AISystem::BFS(int startRow, int startCol, int endRow, int endCol) {
    path = stack<pair<int, int>>();
    vis[ROW-1][COL-1] = {false};

    std::cout << "AI x : " << startRow << " AI y : " << startCol << " \n";
    std::cout << "player x : " << endRow << " player y : " << endCol << " \n";
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
//        std::cout << "curr row : " << x << " curr col : " << y << " \n";
        if(x == endRow && y == endCol) {
            std::cout <<"reached the goal" << " \n";
            findPath(startRow, startCol, endRow, endCol);
//            std::cout << "total path length " << path.size() << " \n";
//            while(!path.empty()) {
//                std::cout << "path x : " << path.top().first << " path y : " << path.top().second << " \n";
//                path.pop();
//            }
            return;
        }

//        path.push_back(q.front());
        q.pop();

        // Go to the adjacent cells
        for (int i = 0; i < 4; i++) {
            int adjx = x + dRow[i];
            int adjy = y + dCol[i];
//            std::cout <<"inside BFS route x " << route[x][y].first << " rount y :"<< route[x][y].second<< " \n";
            if (isValid(adjx, adjy)) {
                q.push({ adjx, adjy });
                vis[adjx][adjy] = true;
                route[adjx][adjy] = {x,y};
            }
        }
    }
}
