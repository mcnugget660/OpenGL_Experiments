#ifndef LEARNOPENGL_CLION_INPUT_H
#define LEARNOPENGL_CLION_INPUT_H

#include <unordered_map>
#include <vector>
#include "Keybind.h"

class Input {

public:

    void registerKey(Keybind *bind);

    void setEnabled(bool value);

    bool isEnabled() const;

    void setKey(int key, int action);

private:
    std::unordered_map<int, std::vector<Keybind *>> keys;
    bool enabled = true;
};


#endif //LEARNOPENGL_CLION_INPUT_H
