#include "ai_system.hpp"
#include <queue>


void AISystem::step(float elapsed_ms, int currentMap)
{
//    (void)elapsed_ms; // placeholder to silence unused warning until implemented
    this->current_map = currentMap;
}


// Direction vectors
int dRow[] = { -1, 0, 1, 0, 1, 1, -1, -1 };
int dCol[] = { 0, -1, 0, 1 ,1, -1, 1, -1 };


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
    if (startRow == endRow && startCol == endCol) return path;
    int x = endRow, y = endCol;
    //    std::cout <<" hello" << endl;
    int counter = 5;
    while (route[x][y].first != startRow || route[x][y].second != startCol) {
        //std::cout << " hello" << endl;
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
    vis[ROW - 1][COL - 1] = { false };

    //std::cout << "AI x : " << startRow << " AI y : " << startCol << " \n";
    //std::cout << "player x : " << endRow << " player y : " << endCol << " \n";
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
        if (x == endRow && y == endCol) {
            // std::cout << "reached the goal" << " \n";
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
                route[adjx][adjy] = { x,y };
            }
        }
    }
}




// adopt from https://dev.to/jansonsa/a-star-a-path-finding-c-4a4h

bool AISystem::isDestination(int x, int y, Node dest) {
    if (x == dest.x && y == dest.y) {
        return true;
    }
    return false;
}

double AISystem::calculateH(int x, int y, Node dest) {
    double H = (sqrt((x - dest.x) * (x - dest.x)
        + (y - dest.y) * (y - dest.y)));
    return H;
}

vector<Node>AISystem::makePath(array<array<Node, (50)>, (50)> map, Node dest) {
    vector<Node> usablePath;
    try {
        //cout << "Found a path" << endl;
        int x = dest.x;
        int y = dest.y;
        stack<Node> path;


        while (!(map[x][y].parentX == x && map[x][y].parentY == y)
            && map[x][y].x != -1 && map[x][y].y != -1)
        {
            path.push(map[x][y]);
            int tempX = map[x][y].parentX;
            int tempY = map[x][y].parentY;
            x = tempX;
            y = tempY;

        }
        path.push(map[x][y]);

        while (!path.empty()) {
            Node top = path.top();
            path.pop();
            usablePath.emplace_back(top);
        }
        return usablePath;
    }
    catch (const exception& e) {
        //cout << e.what() << endl;
        return usablePath;
    }
}

vector<Node> AISystem::aStar(Node player, Node dest) {
    vector<Node> empty;
    if (isValid(dest.x, dest.y) == false) {
        //cout << "Destination is an obstacle" << endl;
        return empty;
        //Destination is invalid
    }
    if (isDestination(player.x, player.y, dest)) {
        // cout << "You are the destination" << endl;
        return empty;
        //You clicked on yourself
    }
    bool closedList[50][50];

    //Initialize whole map
    //Node allMap[50][50];
    array<array < Node, (50)>, (50)> allMap;
    for (int x = 0; x < (50); x++) {
        for (int y = 0; y < (50); y++) {
            allMap[x][y].fCost = FLT_MAX;
            allMap[x][y].gCost = FLT_MAX;
            allMap[x][y].hCost = FLT_MAX;
            allMap[x][y].parentX = -1;
            allMap[x][y].parentY = -1;
            allMap[x][y].x = x;
            allMap[x][y].y = y;

            closedList[x][y] = false;
        }
    }

    //Initialize our starting list
    int x = player.x;
    int y = player.y;
    allMap[x][y].fCost = 0.0;
    allMap[x][y].gCost = 0.0;
    allMap[x][y].hCost = 0.0;
    allMap[x][y].parentX = x;
    allMap[x][y].parentY = y;

    vector<Node> openList;
    openList.emplace_back(allMap[x][y]);
    bool destinationFound = false;

    while (!openList.empty() && openList.size() < (50) * (50)) {
        Node node;
        do {
            //This do-while loop could be replaced with extracting the first
            //element from a set, but you'd have to make the openList a set.
            //To be completely honest, I don't remember the reason why I do
            //it with a vector, but for now it's still an option, although
            //not as good as a set performance wise.
            float temp = FLT_MAX;
            vector<Node>::iterator itNode;
            for (vector<Node>::iterator it = openList.begin();
                it != openList.end(); it = next(it)) {
                Node n = *it;
                if (n.fCost < temp) {
                    temp = n.fCost;
                    itNode = it;
                }
            }
            node = *itNode;
            openList.erase(itNode);
        } while (isValid(node.x, node.y) == false);

        x = node.x;
        y = node.y;
        closedList[x][y] = true;

        //For each neighbour starting from North-West to South-East
        for (int newX = -1; newX <= 1; newX++) {
            for (int newY = -1; newY <= 1; newY++) {
                double gNew, hNew, fNew;
                if (isValid(x + newX, y + newY)) {
                    if (isDestination(x + newX, y + newY, dest))
                    {
                        //Destination found - make path
                        allMap[x + newX][y + newY].parentX = x;
                        allMap[x + newX][y + newY].parentY = y;
                        destinationFound = true;
                        return makePath(allMap, dest);
                    }
                    else if (closedList[x + newX][y + newY] == false)
                    {
                        gNew = node.gCost + 1.0;
                        hNew = calculateH(x + newX, y + newY, dest);
                        fNew = gNew + hNew;
                        // Check if this path is better than the one already present
                        if (allMap[x + newX][y + newY].fCost == FLT_MAX ||
                            allMap[x + newX][y + newY].fCost > fNew)
                        {
                            // Update the details of this neighbour node
                            allMap[x + newX][y + newY].fCost = fNew;
                            allMap[x + newX][y + newY].gCost = gNew;
                            allMap[x + newX][y + newY].hCost = hNew;
                            allMap[x + newX][y + newY].parentX = x;
                            allMap[x + newX][y + newY].parentY = y;
                            openList.emplace_back(allMap[x + newX][y + newY]);
                        }
                    }
                }
            }
        }
    }
    if (destinationFound == false) {
        cout << "Destination not found" << endl;
        return empty;
    }
    return empty;
}