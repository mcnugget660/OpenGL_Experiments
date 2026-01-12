#include "InputManager.h"
#include "GLFW/glfw3.h"
#include "Input.h"

#include <chrono>
#include <algorithm>

auto t = std::chrono::system_clock::now();

bool InputManager::textInputEnabled{false};
std::vector<Input *> InputManager::inputManagers;
std::vector<unsigned int> InputManager::characterQueue;
double InputManager::mouseX{0};
double InputManager::mouseY{0};

void InputManager::setUp(GLFWwindow *window) {
    glfwSetKeyCallback(window, InputManager::key_callback);
    glfwSetCharCallback(window, InputManager::character_callback);
    glfwSetCursorPosCallback(window, InputManager::cursor_position_callback);

    glfwGetCursorPos(window, &mouseX, &mouseY);
}

void InputManager::addManager(Input *manager) {
    inputManagers.push_back(manager);
}

void InputManager::removeManager(Input *manager) {
    inputManagers.erase(std::find(inputManagers.begin(), inputManagers.end(), manager));
}

void InputManager::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    for (Input *manager: inputManagers) {
        if (manager->isEnabled()) {
            manager->setKey(key, action);
        }
    }
}

void InputManager::character_callback(GLFWwindow *window, unsigned int codepoint) {
    if (textInputEnabled) {
        characterQueue.push_back(codepoint);
    }
}

void InputManager::cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    mouseX = xpos;
    mouseY = ypos;
}

std::vector<unsigned int> InputManager::getCharacters() {
    std::vector<unsigned int> copy(characterQueue);
    characterQueue.clear();
    if (copy.size() > 20) // Pointless, but what if it was filled with millions of ints?
        characterQueue.shrink_to_fit();
    return copy;
}

void InputManager::pollEvents() {
    glfwPollEvents();
}

double InputManager::getMouseX() {
    return mouseX;
}

double InputManager::getMouseY() {
    return mouseY;
}

void InputManager::setMouseX(double x) {
    mouseX = x;
}

void InputManager::setMouseY(double y) {
    mouseY = y;
}