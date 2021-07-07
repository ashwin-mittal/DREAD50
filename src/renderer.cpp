#include "renderer.h"

#include <cmath>
#include <iostream>
#include <vector>

#define PI 3.14159

const char *GetGLErrorStr(GLenum err) {
    switch (err) {
        case GL_NO_ERROR:
            return "No error";
        case GL_INVALID_ENUM:
            return "Invalid enum";
        case GL_INVALID_VALUE:
            return "Invalid value";
        case GL_INVALID_OPERATION:
            return "Invalid operation";
            // case GL_STACK_OVERFLOW:
            // return "Stack overflow";
            // case GL_STACK_UNDERFLOW:
            // return "Stack underflow";
        case GL_OUT_OF_MEMORY:
            return "Out of memory";
        default:
            return "Unknown error";
    }
}

void CheckGLError() {
    while (true) {
        const GLenum err = glGetError();
        if (GL_NO_ERROR == err) {
            break;
        }
        std::cout << "GL Error: " << GetGLErrorStr(err) << std::endl;
    }
}

Renderer::Renderer(Shader &shader) {
    this->shader = shader;
    this->initRenderData();
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &this->quadVAO[0]);
    glDeleteVertexArrays(1, &this->quadVAO[1]);
    glDeleteVertexArrays(1, &this->quadVAO[2]);
    glDeleteVertexArrays(1, &this->quadVAO[3]);
}

void Renderer::Draw(const std::string &shape, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color) {
    // prepare transformations
    this->shader.Use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    this->shader.SetMatrix4("model", model);
    this->shader.SetVector3f("u_Color", color);

    if (shape == "RECTANGLE") {
        glBindVertexArray(this->quadVAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glBindVertexArray(this->quadVAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, this->triangles0 / 2);
        glBindVertexArray(0);

        this->shader.SetVector3f("u_Color", glm::vec3(0.0f, 0.5f, 0.5f));
        glBindVertexArray(this->quadVAO[2]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glBindVertexArray(this->quadVAO[3]);
        glDrawArrays(GL_TRIANGLES, 0, this->triangles1);
        glBindVertexArray(0);
    }
    if (shape == "CIRCLE") {
        this->shader.SetVector3f("u_Color", color);
        glBindVertexArray(this->quadVAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, this->triangles0);
        glBindVertexArray(0);
        // this->shader.SetVector3f("u_Color", color);
        // glBindVertexArray(this->quadVAO[1]);
        // glDrawArrays(GL_TRIANGLES, 0, this->triangles0);
        // glBindVertexArray(0);
    }
}

void Renderer::initRenderData() {
    unsigned int VBO;
    float vertices[] = {
        0.0f, 1.5f,
        1.0f, 0.5f,
        0.0f, 0.5f,

        0.0f, 1.5f,
        1.0f, 1.5f,
        1.0f, 0.5f};

    float helmet[] = {
        0.50f, 0.50f,
        0.50f, 0.75f,
        1.00f, 0.75f,

        1.00f, 0.75f,
        1.00f, 0.50f,
        0.50f, 0.50f};

    this->triangles0 = 0;

    std::vector<float> circle, circles;
    float radius = 0.5f;
    float center = 0.5f;

    for (float i = 0; i <= 2 * PI; i += 0.001) {
        if (i > 0) {
            this->triangles0 += 3;
            circle.push_back(center);
            circle.push_back(center);
            circle.push_back(center + (std::cos(i) * radius));
            circle.push_back(center - (std::sin(i) * radius));
        }
        circle.push_back(center + (std::cos(i) * radius));
        circle.push_back(center - (std::sin(i) * radius));
    }

    center = 0.25f;
    radius = 0.25f;
    for (float i = 0; i <= PI; i += 0.001) {
        if (i > 0) {
            this->triangles1 += 3;
            circles.push_back(center);
            circles.push_back(1.5);
            circles.push_back(center + (std::cos(i) * radius));
            circles.push_back(1.5 + (std::sin(i) * radius));
        }
        circles.push_back(center + (std::cos(i) * radius));
        circles.push_back(1.5 + (std::sin(i) * radius));
    }

    circles.pop_back();
    circles.pop_back();

    center = 0.75;

    for (float i = 0; i <= PI; i += 0.001) {
        if (i > 0) {
            this->triangles1 += 3;
            circles.push_back(center);
            circles.push_back(1.5);
            circles.push_back(center + (std::cos(i) * radius));
            circles.push_back(1.5 + (std::sin(i) * radius));
        }
        circles.push_back(center + (std::cos(i) * radius));
        circles.push_back(1.5 + (std::sin(i) * radius));
    }

    circles.pop_back();
    circles.pop_back();

    glGenVertexArrays(1, &this->quadVAO[0]);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //    -----------------------------------------------------------------------------
    glGenVertexArrays(1, &this->quadVAO[1]);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, circle.size() * sizeof(float), &circle[0], GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO[1]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //    -----------------------------------------------------------------------------
    glGenVertexArrays(1, &this->quadVAO[2]);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(helmet), helmet, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO[2]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //    -----------------------------------------------------------------------------
    glGenVertexArrays(1, &this->quadVAO[3]);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * circles.size(), &circles[0], GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO[3]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}