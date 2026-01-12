#ifndef LEARNOPENGL_CLION_INPUTMANAGER_H
#define LEARNOPENGL_CLION_INPUTMANAGER_H

#include <vector>

class GLFWwindow;

class Input;

class InputManager {

public:
    static bool textInputEnabled;

    static void addManager(Input *manager);

    static void removeManager(Input *manager);

    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void character_callback(GLFWwindow *window, unsigned int codepoint);

    static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);

    static std::vector<unsigned int> getCharacters();

    static void pollEvents();

    static void setUp(GLFWwindow *window);

    static double getMouseX();

    static double getMouseY();

    void setMouseX(double x);

    void setMouseY(double y);

private:
    static std::vector<Input *> inputManagers;

    static std::vector<unsigned int> characterQueue;

    static double mouseX;

    static double mouseY;
};


#endif //LEARNOPENGL_CLION_INPUTMANAGER_H
