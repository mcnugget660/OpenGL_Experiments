#include "Input.h"
#include "Keybind.h"

bool Input::isEnabled() const {
    return enabled;
}

void Input::setKey(int key, int action) {
    auto itr = keys.find(key);
    if(itr != keys.end()){
        for(Keybind* bind : itr->second){
            bind->setState(action);
        }
    }
}


void Input::registerKey(Keybind *bind) {
    auto itr = keys.find(bind->getKeyCode());
    if(itr != keys.end()) {
        keys[bind->getKeyCode()].push_back(bind);
    } else {
        std::vector<Keybind *> vec;
        vec.push_back(bind);
        keys[bind->getKeyCode()] = vec; // Vectors store elements on the heap
    }
}

void Input::setEnabled(bool value) {
    enabled = value;
}
