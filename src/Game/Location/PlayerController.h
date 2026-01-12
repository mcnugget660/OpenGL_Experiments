#ifndef FINALDAYONEARTH_PLAYERCONTROLLER_H
#define FINALDAYONEARTH_PLAYERCONTROLLER_H


#include "../../Utility/Input.h"
#include "../../Scene/Camera.h"

class PlayerController : public Input {
public:
    PlayerController();

    void HandleInput(Camera &camera, double deltaTime);

    void reset();
private:
    Keybind W;
    Keybind A;
    Keybind S;
    Keybind D;

    Keybind Q;
    Keybind E;
    Keybind R;
    Keybind Z;
    Keybind X;
    Keybind C;
    Keybind V;

    Keybind Shift;
    Keybind Alt;

    double lastMouseX;
    double lastMouseY;
};


#endif //FINALDAYONEARTH_PLAYERCONTROLLER_H
