#ifndef LEARNOPENGL_CLION_KEYBIND_H
#define LEARNOPENGL_CLION_KEYBIND_H

#include <string>

class Keybind {
public:
    Keybind(int keycode, std::string name, bool sticky = false);

    int getKeyCode() const;

    void setKeyCode(int keyCode);

    const std::string &getName() const;

    void setName(const std::string &name);

    bool isSticky() const;

    void setSticky(bool sticky);

    bool wasPressed();

    bool isPressed() const;

    void setState(int state);

private:
    int keyCode = -1;
    std::string name = "nothing";
    bool sticky = false;

    int presses = 0;
    int currentState = -1;
    bool toggle = false;

    void press();
    void release();
};


#endif //LEARNOPENGL_CLION_KEYBIND_H
