#include "PlayerController.h"
#include "GLFW/glfw3.h"
#include "../../Utility/InputManager.h"

PlayerController::PlayerController() : W(GLFW_KEY_W, "roll"),
                                       A(GLFW_KEY_A, "roll"),
                                       S(GLFW_KEY_S, "roll"),
                                       D(GLFW_KEY_D, "roll"),

                                       Q(GLFW_KEY_Q, "roll"),
                                       E(GLFW_KEY_E, "roll"),
                                       R(GLFW_KEY_R, "roll"),
                                       Z(GLFW_KEY_Z, "roll"),
                                       X(GLFW_KEY_X, "roll"),
                                       C(GLFW_KEY_C, "zoom"),
                                       Shift(GLFW_KEY_LEFT_SHIFT, "shift"),
                                       V(GLFW_KEY_V, "roll"),
                                       Alt(GLFW_KEY_LEFT_ALT, "alt"){
    registerKey(&W);
    registerKey(&A);
    registerKey(&S);
    registerKey(&D);

    registerKey(&Q);
    registerKey(&E);
    registerKey(&R);
    registerKey(&Z);
    registerKey(&X);
    registerKey(&C);
    registerKey(&V);

    registerKey(&Shift);
    registerKey(&Alt);
}

void PlayerController::HandleInput(Camera &camera, double deltaTime) {
    double mouseSensitivity = 1.4;
    double movementSpeed = 1.0 + Shift.isPressed() * 9 + Shift.isPressed() * Q.isPressed() * 900;
    if(Alt.isPressed())
        movementSpeed *= 0.01;

    if (W.isPressed())
        camera.move(FORWARDS, movementSpeed * deltaTime);

    if (A.isPressed())
        camera.move(LEFT, movementSpeed * deltaTime);

    if (S.isPressed())
        camera.move(BACKWARDS, movementSpeed * deltaTime);

    if (D.isPressed())
        camera.move(RIGHT, movementSpeed * deltaTime);

    if (Z.isPressed())
        camera.move(DOWN, movementSpeed * deltaTime);

    if (X.isPressed())
        camera.move(UP, movementSpeed * deltaTime);


    camera.setZoom(1 + C.isPressed());

    camera.rotate(mouseSensitivity * deltaTime * (InputManager::getMouseX() - lastMouseX),
                          -mouseSensitivity * deltaTime * (InputManager::getMouseY() - lastMouseY));

    lastMouseX = InputManager::getMouseX();
    lastMouseY = InputManager::getMouseY();
}

void PlayerController::reset() {
    lastMouseX = InputManager::getMouseX();
    lastMouseY = InputManager::getMouseY();
}
