//
// Created by ashwin on 29/03/21.
//
#include "player.h"

#include <utility>

Player::Player() : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), Shape("RECTANGLE"), IsSolid(false), Destroyed(false) {}

Player::Player(glm::vec2 pos, glm::vec2 size, std::string shape, glm::vec3 color, glm::vec2 velocity) : Position(pos),
                                                                                                        Size(size),
                                                                                                        Velocity(velocity),
                                                                                                        Color(color),
                                                                                                        Rotation(0.0f),
                                                                                                        Shape(std::move(shape)),
                                                                                                        IsSolid(false),
                                                                                                        Destroyed(false) {}

void Player::Draw(Renderer &renderer) {
    renderer.Draw(this->Shape, this->Position, glm::vec2(this->Size.x, (float)this->Size.y / 1.75), this->Rotation, this->Color);
}
