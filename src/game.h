#ifndef GAME_H
#define GAME_H

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "game_level.h"
#include "powerup.h"

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_OVER
};

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(150.0f);

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game {
   public:
    // game state
    GameState State;
    bool Keys[1024];
    unsigned int Width, Height;
    GameLevel Level;
    unsigned int Lives;
    unsigned int Light;
    unsigned int Tasks;
    int Score;
    std::vector<Powerup> Powerups;
    std::vector<std::pair<int, int>> empty;

    float Time;

    // constructor/destructor
    Game(unsigned int width, unsigned int height);

    ~Game();

    // initialize game state (load all shaders/textures/levels)
    void Init();

    void ResetPlayer() const;

    // game loop
    void ProcessInput(float dt);

    template <class T>
    void collisions(float dt, T &player);

    void move_imposter(float dt);

    template <class t1, class t2>
    bool collides(t1 &one, t2 &two);

    void Update(GLFWwindow *window, float dt);

    void Render();
};

#endif