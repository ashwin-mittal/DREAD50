#include "game.h"

#include <GLFW/glfw3.h>

#include <iostream>
#include <queue>
#include <sstream>
#include <utility>

#include "player.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "text_renderer.h"

// Game-related State data
SpriteRenderer *Sprite_Renderer;
Renderer *renderer;
TextRenderer *Text;
Player *player;
Player *imposter;
Powerup *imposter_button;
Powerup *release;

bool ShouldSpawn(unsigned int chance) {
    unsigned int random = rand() % chance;
    return random == 0;
}

void Game::ResetPlayer() const {
    player->Position = glm::vec2(0.0f, (float)this->Height / this->Level.MazeHeight + (float)this->Height / this->Level.MazeHeight - player->Size.y);
}

Game::Game(unsigned int width, unsigned int height) : State(GAME_ACTIVE), Keys(), Width(width), Height(height), Lives(5), Light(1), Time(1000), Tasks(0), Score(0) {
}

Game::~Game() {
    delete Sprite_Renderer;
    delete renderer;
    delete Text;
    delete player;
    delete imposter;
    delete imposter_button;
    delete release;
}

void Game::Init() {
    // load shaders
    ResourceManager::LoadShader("../src/shaders/shader.vs", "../src/shaders/shader.fs", nullptr, "sprite");
    ResourceManager::LoadShader("../src/shaders/player_shader.vs", "../src/shaders/player_shader.fs", nullptr, "player");

    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("player").Use().SetMatrix4("projection", projection);

    // set render-specific controls
    Sprite_Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    renderer = new Renderer(ResourceManager::GetShader("player"));

    // load textures
    ResourceManager::LoadTexture("../assets/textures/awesomeface.png", true, "face");
    ResourceManager::LoadTexture("../assets/textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("../assets/textures/block_solid.png", false, "block");

    this->Level.Load("../assets/maze.txt", this->Width, this->Height);
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("../assets/fonts/font.ttf", 24);

    player = new Player(glm::vec2(0.0f, (float)this->Height / this->Level.MazeHeight + (float)this->Height / this->Level.MazeHeight - (float)this->Height / (this->Level.MazeHeight + 5)),
                        glm::vec2((float)this->Width / (this->Level.MazeWidth + 20), (float)this->Height / (this->Level.MazeHeight + 5)),
                        "RECTANGLE",
                        glm::vec3(0.0f, 1.0f, 0.0f));

    imposter = new Player(glm::vec2(this->Width - (float)this->Width / (this->Level.MazeWidth + 20), (float)this->Height / this->Level.MazeHeight * (this->Level.MazeHeight - 1) - (float)this->Height / (this->Level.MazeHeight + 5)),
                          glm::vec2((float)this->Width / (this->Level.MazeWidth + 20), (float)this->Height / (this->Level.MazeHeight + 5)),
                          "RECTANGLE",
                          glm::vec3(1.0f, 0.0f, 0.0f));

    std::vector<std::string> maze = this->Level.MazeData;
    unsigned int height = maze.size();
    unsigned int width = maze[0].size();

    float unit_width = this->Width / static_cast<float>(width), unit_height = (float)this->Height / height;

    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            if (maze[y][x] == ' ') {
                empty.push_back({y, x});
                if (ShouldSpawn(20)) {
                    this->Powerups.push_back(Powerup("CIRCLE",
                                                     ShouldSpawn(2) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(0.5f, 0.5f, 0.5f),
                                                     glm::vec2(unit_width * x, unit_height * y),
                                                     glm::vec2(unit_width, unit_height)));
                }
            }
        }
    }

    std::pair<int, int> chosen = empty[rand() % empty.size()];

    imposter_button = new Powerup("CIRCLE", glm::vec3(1.0f, 0.0f, 0.0f),
                                  glm::vec2(unit_width * chosen.second, unit_height * chosen.first),
                                  glm::vec2(unit_width, unit_height));

    chosen = empty[rand() % empty.size()];
    release = new Powerup("CIRCLE", glm::vec3(0.0f, 1.0f, 0.0f),
                          glm::vec2(unit_width * chosen.second, unit_height * chosen.first),
                          glm::vec2(unit_width, unit_height));
}

void Game::Update(GLFWwindow *window, float dt) {
    if (this->State == GAME_ACTIVE) {
        this->Time -= dt;
        if (this->Time <= 0) {
            this->Time = 0;
            this->State = GAME_OVER;
        }
        collisions(dt, *player);
        move_imposter(dt);
        collisions(dt, *imposter);
        for (Powerup &powerup : this->Powerups) {
            if (powerup.Activated) {
                if (collides(*player, powerup)) {
                    if (powerup.Color.x == 1.0f) {
                        if (!release->Activated) {
                            powerup.Activated = false;
                            Score += 50 * (2 - Light);
                        }
                    } else {
                        powerup.Activated = false;
                        Score -= 50;
                    }
                }
            }
        }

        if (collides(*player, *imposter_button) && imposter_button->Activated) {
            Tasks += 1;
            Score += 100 * (2 - Light);
            imposter_button->Activated = false;
        }

        if (collides(*player, *release) && release->Activated) {
            Tasks += 1;
            Score += 100 * (2 - Light);
            release->Activated = false;
        }

        if (collides(*imposter, *imposter_button) && imposter_button->Activated) {
            std::pair<int, int> chosen = empty[rand() % empty.size()];
            imposter->Position.x = chosen.second * (float)this->Width / this->Level.MazeWidth + 1e-3;
            imposter->Position.y = chosen.first * (float)this->Height / this->Level.MazeHeight + 1e-3;
            imposter->Velocity = glm::vec2(0.0f);
        }

        if (collides(*player, *imposter)) {
            ResetPlayer();
            Lives -= 1;
            if (Lives == 0) {
                State = GAME_OVER;
            }
        }
        ResourceManager::GetShader("player").Use().SetInteger("light", Light);
        ResourceManager::GetShader("sprite").Use().SetInteger("light", Light);
        ResourceManager::GetShader("player").Use().SetVector2f("position", glm::vec2(player->Position.x + player->Size.x / 2, player->Position.y + player->Size.y / 2));
        ResourceManager::GetShader("sprite").Use().SetVector2f("position", glm::vec2(player->Position.x + player->Size.x / 2, player->Position.y + player->Size.y / 2));
    }
}

void Game::move_imposter(float dt) {
    std::vector<std::string> maze = this->Level.MazeData;
    unsigned int height = maze.size();
    unsigned int width = maze[0].size();
    std::pair<int, int> imposter_position;
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    std::vector<std::vector<int>> distance(height, std::vector<int>(width, 1e9));
    std::vector<std::vector<std::pair<int, int>>> trace(height, std::vector<std::pair<int, int>>(width));

    std::priority_queue<std::pair<int, std::pair<int, int>>, std::vector<std::pair<int, std::pair<int, int>>>, std::greater<std::pair<int, std::pair<int, int>>>> pq;

    float unit_width = this->Width / static_cast<float>(width), unit_height = (float)this->Height / height;
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            if (maze[y][x] == ' ') {
                glm::vec2 pos(unit_width * x, unit_height * y);
                if (player->Position.x >= pos.x && player->Position.x < pos.x + unit_width &&
                    player->Position.y >= pos.y && player->Position.y < pos.y + unit_height) {
                    distance[y][x] = 0;
                    trace[y][x] = {y, x};
                    pq.push({0, {y, x}});
                }
                if (imposter->Position.x >= pos.x && imposter->Position.x < pos.x + unit_width &&
                    imposter->Position.y >= pos.y && imposter->Position.y < pos.y + unit_height) {
                    if (imposter->Position.x + imposter->Size.x > pos.x + unit_width ||
                        imposter->Position.y + imposter->Size.y > pos.y + unit_height) {
                        imposter->Position.y += imposter->Velocity.y * dt;
                        imposter->Position.x += imposter->Velocity.x * dt;
                        return;
                    }
                    imposter_position = {y, x};
                }
            }
        }
    }
    trace[imposter_position.first][imposter_position.second] = {imposter_position.first, imposter_position.second};

    std::vector<std::pair<int, int>> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    while (!pq.empty()) {
        std::pair<int, std::pair<int, int>> node = pq.top();
        pq.pop();
        if (visited[node.second.first][node.second.second]) {
            continue;
        }
        visited[node.second.first][node.second.second] = true;
        int y = node.second.first;
        int x = node.second.second;
        for (std::pair<int, int> direction : directions) {
            if (y + direction.first >= 0 && y + direction.first < height && x + direction.second >= 0 &&
                x + direction.second < width) {
                if (maze[y + direction.first][x + direction.second] == ' ') {
                    if (distance[y + direction.first][x + direction.second] > distance[y][x] + 1) {
                        distance[y + direction.first][x + direction.second] = distance[y][x] + 1;
                        trace[y + direction.first][x + direction.second] = {y, x};
                        pq.push({distance[y + direction.first][x + direction.second], {y + direction.first, x + direction.second}});
                    }
                }
            }
        }
    }
    //        std::cout << imposter_position.first << " " << imposter_position.second << "\n";
    //        std::cout << trace[imposter_position.first][imposter_position.second].first - imposter_position.first << " "
    //                  << trace[imposter_position.first][imposter_position.second].second - imposter_position.second << distance[imposter_position.first][imposter_position.second] <<  "\n";
    imposter->Velocity.y = (trace[imposter_position.first][imposter_position.second].first - imposter_position.first) * unit_height;
    imposter->Velocity.x = (trace[imposter_position.first][imposter_position.second].second - imposter_position.second) * unit_width;

    imposter->Position.y += imposter->Velocity.y * dt;
    imposter->Position.x += imposter->Velocity.x * dt;
}

void Game::ProcessInput(float dt) {
    player->Velocity.x = 0;
    player->Velocity.y = 0;

    if (this->State == GAME_ACTIVE) {
        float velocity = PLAYER_VELOCITY * dt;
        if (this->Keys[GLFW_KEY_LEFT]) {
            if (player->Position.x >= 0.0f) {
                player->Position.x -= velocity;
                player->Velocity.x = -PLAYER_VELOCITY;
            }
        }
        if (this->Keys[GLFW_KEY_RIGHT]) {
            // if (player->Position.x <= this->Width - player->Size.x) {
            player->Position.x += velocity;
            player->Velocity.x = PLAYER_VELOCITY;
            if (player->Position.x >= this->Width && Tasks == 2) {
                this->State = GAME_OVER;
            }
            // }
        }
        if (this->Keys[GLFW_KEY_UP]) {
            if (player->Position.y >= 0.0f) {
                player->Position.y -= velocity;
                player->Velocity.y = -PLAYER_VELOCITY;
            }
        }
        if (this->Keys[GLFW_KEY_DOWN]) {
            if (player->Position.y <= this->Height - player->Size.y) {
                player->Position.y += velocity;
                player->Velocity.y = PLAYER_VELOCITY;
            }
        }
        if (this->Keys[GLFW_KEY_SPACE]) {
            Light = !Light;
            this->Keys[GLFW_KEY_SPACE] = false;
        }
    }
}

void Game::Render() {
    if (this->State == GAME_ACTIVE || this->State == GAME_OVER) {
        Sprite_Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);

        this->Level.Draw(*Sprite_Renderer);
        player->Draw(*renderer);
        imposter->Draw(*renderer);
        std::stringstream ss;
        ss << this->Lives;
        Text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);

        ss.str(std::string());
        ss << this->Light;
        Text->RenderText("Light:" + ss.str(), 5.0f, 25.0f, 1.0f);

        ss.str(std::string());
        ss << round(this->Time);
        Text->RenderText("Time:" + ss.str(), this->Width - 125.0f, 5.0f, 1.0f);

        ss.str(std::string());
        ss << this->Tasks;
        Text->RenderText("Tasks:" + ss.str(), this->Width - 125.0f, 25.0f, 1.0f);

        ss.str(std::string());
        ss << this->Score;
        Text->RenderText("Score:" + ss.str(), this->Width / 2 - 30.0f, 5.0f, 1.0f);

        Text->RenderText("Exit", this->Width - 50.0f, ((float)this->Height / this->Level.MazeHeight) * (this->Level.MazeHeight - 2) - 25.0f, 1.0f);

        for (Powerup &powerup : this->Powerups) {
            if (powerup.Activated) {
                powerup.Draw(*renderer);
                ss.str(std::string());
                ss << (powerup.Color.x == 1.0f ? 50 : -50);
                Text->RenderText(ss.str(), powerup.Position.x + powerup.Size.x / 2 - 20.0f, powerup.Position.y + powerup.Size.y / 2 - 5.0f, 1.0f);
            }
        }

        if (imposter_button->Activated) {
            ss.str(std::string());
            ss << this->Score;
            imposter_button->Draw(*renderer);
            ss.str(std::string());
            ss << ("Press");
            Text->RenderText(ss.str(), imposter_button->Position.x + imposter_button->Size.x / 2 - 25.0f, imposter_button->Position.y + imposter_button->Size.y / 2 - 5.0f, 0.75f);
        }

        if (release->Activated) {
            ss.str(std::string());
            ss << this->Score;
            release->Draw(*renderer);
            ss.str(std::string());
            ss << ("Release");
            Text->RenderText(ss.str(), release->Position.x + release->Size.x / 2 - 25.0f, release->Position.y + release->Size.y / 2 - 5.0f, 0.5f);
        }
    }
    if (this->State == GAME_OVER) {
        Text->RenderText("GAME OVER", this->Width / 2 - 30.0f, this->Height / 2.0f - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        Text->RenderText("PRESS ESC TO QUIT", this->Width / 2 - 100.0f, this->Height / 2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    }
}

// AABB - AABB collision
template <class t1, class t2>
bool Game::collides(t1 &one, t2 &two) {
    if (one.Position.x + one.Size.x <= two.Position.x || abs(one.Position.x + one.Size.x - two.Position.x) <= 1e-3 ||
        two.Position.x + two.Size.x <= one.Position.x || abs(two.Position.x + two.Size.x - one.Position.x) <= 1e-3) {
        return false;
    }
    if (one.Position.y + one.Size.y <= two.Position.y || abs(one.Position.y + one.Size.y - two.Position.y) <= 1e-3 ||
        two.Position.y + two.Size.y <= one.Position.y || abs(two.Position.y + two.Size.y - one.Position.y) <= 1e-3) {
        return false;
    }
    return true;
}

template <class T>
void Game::collisions(float dt, T &player) {
    for (GameObject &block : this->Level.Maze) {
        if (!collides(player, block)) {
            continue;
        }
        // std::cout << player.Position.x << ' ' << player.Position.y  << ' ' << player.Size.y<< "\n";
        // std::cout << block.Position.x << ' ' << block.Position.y << "\n";
        // std::cout << abs(player.Position.y + player.Size.y - block.Position.y) << "\n";
        player.Position -= player.Velocity * dt;
        //        if (player.Position.x + player.Size.x / 4 < block.Position.x && player.Velocity.x > 0) {
        //            player.Position.x = block.Position.x - player.Size.x;
        //        } else if (player.Position.x + 3 * player.Size.x / 4 > block.Position.x + block.Size.x &&
        //                   player.Velocity.x < 0) {
        //            player.Position.x = block.Position.x + block.Size.x;
        //        } else if (player.Position.y < block.Position.y) {
        //            player.Position.y = block.Position.y - player.Size.y;
        //        } else {
        //            player.Position.y = block.Position.y + block.Size.y;
        //        }
        break;
    }
}