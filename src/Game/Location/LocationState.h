#ifndef FINALDAYONEARTH_LOCATIONSTATE_H
#define FINALDAYONEARTH_LOCATIONSTATE_H

#include "Location.h"
#include "../../Scene/Camera.h"
#include "../../Rendering/Shader.h"
#include "../../Utility/Input.h"
#include "PlayerController.h"
#include "../../Rendering/ModelRenderer.h"
#include "../../Rendering/LocationRenderer.h"
#include "../Base/GameState.h"

class LocationState : public GameState {
public:
    LocationState();

    void openLocation(Location *location);

    void run(double deltaTime) override;

    void render(double deltaTime) override;

    std::vector<Input *> getInputs() override;

    void updateScreenSize(unsigned int width, unsigned int height) override;

private:
    Location *location = nullptr;

    Camera *currentCamera = nullptr;
    Camera camera;
    Camera camera2;
    bool usingCamera2 = false;

    PlayerController playerController;
    Keybind roll;
    Keybind newCamera;
    Input sceneInputs;

    LocationRenderer renderer;

    //EntityPhysicsSystem entityPhysicsSystem;

    void swapCameras();
};


#endif //FINALDAYONEARTH_LOCATIONSTATE_H
