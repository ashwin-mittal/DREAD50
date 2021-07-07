#include "game_level.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

std::vector<std::string> GameLevel::GetMaze() {
    int MazeWidth;
    int MazeHeight;
    int *maze;

    enum {
        CELL_PATH_N = 0x01,
        CELL_PATH_E = 0x02,
        CELL_PATH_S = 0x04,
        CELL_PATH_W = 0x08,
        CELL_VISITED = 0x10,
    };

    int VisitedCells;
    std::stack<std::pair<int, int>> stack;
    int PathWidth;
    PathWidth = 1;

    MazeWidth = 16;
    MazeHeight = 8;
    maze = new int[MazeWidth * MazeHeight];
    memset(maze, 0x00, MazeWidth * MazeHeight * sizeof(int));

    int x = rand() % MazeWidth;
    int y = rand() % MazeHeight;
    stack.push(std::make_pair(x, y));
    maze[y * MazeWidth + x] = CELL_VISITED;
    VisitedCells = 1;

    while (VisitedCells < MazeWidth * MazeHeight) {
        auto offset = [&](int x, int y) {
            return (stack.top().second + y) * MazeWidth + (stack.top().first + x);
        };

        if (VisitedCells < MazeWidth * MazeHeight) {
            std::vector<int> neighbours;
            if (stack.top().second > 0 && (maze[offset(0, -1)] & CELL_VISITED) == 0) {
                neighbours.push_back(0);
            }
            if (stack.top().first < MazeWidth - 1 && (maze[offset(1, 0)] & CELL_VISITED) == 0) {
                neighbours.push_back(1);
            }
            if (stack.top().second < MazeHeight - 1 && (maze[offset(0, 1)] & CELL_VISITED) == 0) {
                neighbours.push_back(2);
            }
            if (stack.top().first > 0 && (maze[offset(-1, 0)] & CELL_VISITED) == 0) {
                neighbours.push_back(3);
            }
            if (!neighbours.empty()) {
                int next_cell_dir = neighbours[rand() % neighbours.size()];
                switch (next_cell_dir) {
                    case 0:
                        maze[offset(0, -1)] |= CELL_VISITED | CELL_PATH_S;
                        maze[offset(0, 0)] |= CELL_PATH_N;
                        stack.push(std::make_pair((stack.top().first + 0), (stack.top().second - 1)));
                        break;

                    case 1:
                        maze[offset(+1, 0)] |= CELL_VISITED | CELL_PATH_W;
                        maze[offset(0, 0)] |= CELL_PATH_E;
                        stack.push(std::make_pair((stack.top().first + 1), (stack.top().second + 0)));
                        break;

                    case 2:
                        maze[offset(0, +1)] |= CELL_VISITED | CELL_PATH_N;
                        maze[offset(0, 0)] |= CELL_PATH_S;
                        stack.push(std::make_pair((stack.top().first + 0), (stack.top().second + 1)));
                        break;

                    case 3:
                        maze[offset(-1, 0)] |= CELL_VISITED | CELL_PATH_E;
                        maze[offset(0, 0)] |= CELL_PATH_W;
                        stack.push(std::make_pair((stack.top().first - 1), (stack.top().second + 0)));
                        break;
                }
                VisitedCells++;
            } else {
                stack.pop();
            }
        }
    }

    std::vector<std::string> Maze(MazeHeight * (PathWidth + 1) + 1, std::string(MazeWidth * (PathWidth + 1) + 1, '#'));

    for (int x = 0; x < MazeWidth; x++) {
        for (int y = 0; y < MazeHeight; y++) {
            if (maze[y * MazeWidth + x] & CELL_VISITED) {
                Maze[y * (PathWidth + 1) + 1][x * (PathWidth + 1) + 1] = ' ';
            }
            if (maze[y * MazeWidth + x] & CELL_PATH_S) {
                Maze[y * (PathWidth + 1) + PathWidth + 1][x * (PathWidth + 1) + 1] = ' ';
            }
            if (maze[y * MazeWidth + x] & CELL_PATH_E) {
                Maze[y * (PathWidth + 1) + 1][x * (PathWidth + 1) + PathWidth + 1] = ' ';
            }
        }
    }

    Maze[1][0] = ' ';
    Maze[MazeHeight * (PathWidth + 1) - 1][MazeWidth * (PathWidth + 1)] = ' ';

    return Maze;
}

void GameLevel::Load(const char *file, unsigned int levelWidth, unsigned int levelHeight) {
    this->Maze.clear();
    return this->init(levelWidth, levelHeight);
}

void GameLevel::Draw(SpriteRenderer &renderer) {
    for (GameObject &block : this->Maze) {
        block.Draw(renderer);
    }
}

bool GameLevel::IsCompleted() {
    // for (GameObject &tile : this->Bricks) {
    //     if (!tile.IsSolid && !tile.Destroyed) {
    //         return false;
    //     }
    // }
    return true;
}

void GameLevel::init(unsigned int levelWidth, unsigned int levelHeight) {
    std::vector<std::string> mazeData = GetMaze();
    this->MazeData = mazeData;
    unsigned int height = mazeData.size();
    unsigned int width = mazeData[0].size();
    this->MazeWidth = width;
    this->MazeHeight = height;
    float unit_width = levelWidth / static_cast<float>(width), unit_height = (float)levelHeight / height;
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            if (mazeData[y][x] == '#') {
                glm::vec3 color = glm::vec3(1.0f);
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                this->Maze.push_back(GameObject(pos, size, ResourceManager::GetTexture("block"), color));
            }
        }
    }
}