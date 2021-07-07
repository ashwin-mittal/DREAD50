//
// Created by ashwin on 31/03/21.
//

#include "powerup.h"

void Powerup::Draw(Renderer &renderer) {
    if (Activated) {
        renderer.Draw(this->Type, this->Position, this->Size, 0.0f, this->Color);
    }
}
