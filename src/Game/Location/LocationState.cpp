#include <iostream>
#include "LocationState.h"
#include "../../Utility/InputManager.h"
#include "../../Scene/QuadTree.h"
#include "GLFW/glfw3.h"
#include "../../Rendering/Model.h"
#include "../../Utility/ResourceLoader.h"

LocationState::LocationState() : camera(0, 0, 0, 16.0 / 9.0), camera2(0, 0, 0, 16.0 / 9.0), roll(GLFW_KEY_R, "roll"), newCamera(GLFW_KEY_J,"newCamera") {
    sceneInputs.registerKey(&roll);
    sceneInputs.registerKey(&newCamera);
    camera2.zFar = 10000;
}

void LocationState::openLocation(Location *l) {
    location = l;

    camera.setLocation(glm::vec3(l->getSize() / 2, 12.55, l->getSize() / 2));

    currentCamera = &camera;
    usingCamera2 = false;

    location->time = 600;

    playerController.reset();

    renderer.setLocation(location);
}

void LocationState::updateScreenSize(unsigned int width, unsigned int height) {
    GameState::updateScreenSize(width, height);
    camera.aspectRatio = (float) SCR_WIDTH / SCR_HEIGHT;
    camera2.aspectRatio = (float) SCR_WIDTH / SCR_HEIGHT;
}

void LocationState::run(double deltaTime) {
    if (newCamera.wasPressed())
        swapCameras();

    if (roll.isPressed())
        currentCamera->setRoll(camera.getRoll() + 15.0 * deltaTime);

    playerController.HandleInput(*currentCamera, deltaTime);

    location->advanceTime(0, deltaTime * 10);

    // Systems
//    entityPhysicsSystem.execute(location, deltaTime);
}

void LocationState::render(double deltaTime) {
    renderer.renderLocation(camera, usingCamera2 ? camera2 : camera, location->time);
}

std::vector<Input *> LocationState::getInputs() {
    std::vector<Input *> v;
    v.push_back(&playerController);
    v.push_back(&sceneInputs);
    return v;
}

void LocationState::swapCameras() {
    usingCamera2 = !usingCamera2;
    if (usingCamera2) {
        camera2.setLocation(camera.getLocation());
        camera2.setPitch(camera.getPitch());
        camera2.setYaw(camera.getYaw());
        camera2.setRoll(camera.getRoll());
        currentCamera = &camera2;
    } else
        currentCamera = &camera;
}
