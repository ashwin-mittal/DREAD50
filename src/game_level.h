#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <vector>

#include "game_object.h"
#include "resource_manager.h"
#include "sprite_renderer.h"

/// GameLevel holds all Tiles as part of a Dread50 level and
/// hosts functionality to Load/render levels from the hard disk.
class GameLevel {
   public:
    // level state
    std::vector<GameObject> Maze;
    unsigned int MazeWidth;
    unsigned int MazeHeight;

    std::vector<std::string> MazeData;

    // constructor
    GameLevel() {}

    static std::vector<std::string> GetMaze();

    // loads level from file
    void Load(const char *file, unsigned int levelWidth, unsigned int levelHeight);

    // render level
    void Draw(SpriteRenderer &renderer);

    // check if the level is completed (all non-solid tiles are destroyed)
    bool IsCompleted();

   private:
    // initialize level from tile data
    void init(unsigned int levelWidth, unsigned int levelHeight);
};

#endif