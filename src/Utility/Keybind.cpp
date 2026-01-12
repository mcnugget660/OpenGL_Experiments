#include "Keybind.h"
#include "GLFW/glfw3.h"

#include <utility>

Keybind::Keybind(int keycode, std::string name, bool sticky) : keyCode(keycode), name(std::move(name)), sticky(sticky), currentState(GLFW_FALSE){

}

int Keybind::getKeyCode() const {
    return keyCode;
}

void Keybind::setKeyCode(int code) {
    keyCode = code;
}

const std::string &Keybind::getName() const {
    return name;
}

void Keybind::setName(const std::string &n) {
    name = n;
}

bool Keybind::isSticky() const {
    return sticky;
}

void Keybind::setSticky(bool s) {
    if(sticky != s) // True -> False : reset toggle
        toggle = false;
    sticky = s;
}

bool Keybind::wasPressed() {
    if(presses > 0) {
        presses--;
        return true;
    }
    return false;
}

bool Keybind::isPressed() const {
    if(sticky)
        return toggle;
    return currentState != GLFW_RELEASE;
}

void Keybind::setState(int state) {
    if(state == GLFW_RELEASE)
        release();
    else if(state == GLFW_PRESS)
        press();

    currentState = state;
}

void Keybind::release() {
    presses++; // Pressing a button usually refers to releasing it (perhaps this could be an option)

    if(sticky)
        toggle = !toggle;
}

void Keybind::press() {

}
