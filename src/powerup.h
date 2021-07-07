//
// Created by ashwin on 31/03/21.
//

#ifndef GRAPHICS_POWERUP_H
#define GRAPHICS_POWERUP_H

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "renderer.h"

// The size of a PowerUp block
const glm::vec2 POWERUP_SIZE(60.0f, 20.0f);

// PowerUp inherits its state and rendering functions from
// GameObject but also holds extra information to state its
// active duration and whether it is activated or not.
// The type of PowerUp is stored as a string.
class Powerup {
   public:
    // powerup state
    glm::vec2 Position, Size;
    glm::vec3 Color;
    std::string Type;
    bool Activated;

    // constructor
    Powerup(std::string type,
            glm::vec3 color, glm::vec2 position, glm::vec2 size)
        : Type(type), Color(color), Position(position), Size(size), Activated(true) {}

    virtual void Draw(Renderer &renderer);
};

#endif  //GRAPHICS_POWERUP_H
