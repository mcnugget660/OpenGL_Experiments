#include "glm/detail/type_mat4x4.hpp"
#include "GameState.h"

void GameState::run(double deltaTime) {

}

void GameState::render(double deltaTime) {

}

std::vector<Input *> GameState::getInputs() {
    return std::vector<Input *>();
}

void GameState::updateScreenSize(unsigned int width, unsigned int height) {
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}
