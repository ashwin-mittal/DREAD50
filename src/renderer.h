//
// Created by ashwin on 29/03/21.
//

#ifndef GRAPHICS_RENDERER_H
#define GRAPHICS_RENDERER_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "texture.h"

class Renderer {
   public:
    unsigned int triangles0;
    unsigned int triangles1;

    // Constructor (inits shaders/shapes)
    Renderer(Shader &shader);

    // Destructor
    ~Renderer();

    // Renders a defined quad textured with given sprite
    void
    Draw(const std::string &shape, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));

   private:
    // Render state
    Shader shader;
    unsigned int quadVAO[4];

    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();
};

#endif  //GRAPHICS_RENDERER_H
