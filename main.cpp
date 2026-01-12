#include <iostream>
#include <chrono>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "src/Rendering/Window.h"
#include "src/Utility/Keybind.h"
#include "src/Utility/Input.h"
#include "src/Utility/InputManager.h"

#include "src/Rendering/RenderUtil.h"
#include "src/Rendering/TriangleMesh.h"
#include "src/Rendering/Vertices.h"
#include "src/Rendering/Shader.h"
#include "src/Scene/Camera.h"
#include "src/Game/Location/LocationState.h"
#include "src/Game/Location/LocationGenerator.h"
#include "src/Utility/PerlinNoise.h"
#include "src/Utility/ResourceLoader.h"


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

long getTimeMicro();

void run();

int main() {
    run();
}

void run() {
    Window::start();

    Window::buildDefault();

    InputManager::setUp(Window::getWindow());

    Keybind exit(GLFW_KEY_ESCAPE, "exit");
    Keybind toggleMouse(GLFW_KEY_T, "toggleMouse");
    Keybind refreshResources(GLFW_KEY_F1, "refreshResources");

    Input input;

    input.registerKey(&exit);
    input.registerKey(&toggleMouse);
    input.registerKey(&refreshResources);

    InputManager::addManager(&input);

    Window::hideMouse();

    ResourceLoader::loadConfigs();

    LocationState locationState;
    locationState.updateScreenSize(SCR_WIDTH, SCR_HEIGHT);

    for (Input *in: locationState.getInputs())
        InputManager::addManager(in);

    Location *locationTest123 = LocationGenerator::generateRandomLocation(Unknown, Empty, SMALL, 69);

    locationState.openLocation(locationTest123);


    // Wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //Disable V-Sync
    //glfwSwapInterval(0);
    // Transparency
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable( GL_BLEND );

    double totalTime = 0;
    double frameCount = 0;

    long time = getTimeMicro();

    while (!Window::shouldClose()) {
        long now = getTimeMicro();

        double frameTime = (now - time) / 1000000.0; // In seconds

        totalTime += frameTime;
        frameCount++;

        time = now;

        InputManager::pollEvents();

        if (exit.wasPressed())
            break;

        if (toggleMouse.wasPressed())
            Window::toggleMouse();
        if(refreshResources.wasPressed())
            ResourceLoader::refreshResources();

        if (Window::wasResized() || frameCount == 2)
            locationState.updateScreenSize(Window::getWidth(), Window::getHeight());

        // Run
        locationState.run(frameTime);

        RenderUtil::clearBuffer();

        // Render
        locationState.render(frameTime);

        Window::swapBuffer();

        //std::cout << "FPS : " << (1.0 / frameTime) << "\n";
    }

    ResourceLoader::freeAllResources();

    std::cout << "Average FPS : " << (frameCount / totalTime) << "\n";

    Window::showMouse();

    Window::terminate();
}

long getTimeMicro() {
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}