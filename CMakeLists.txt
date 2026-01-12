cmake_minimum_required(VERSION 3.27.8)
project(FinalDayOnEarth)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

# Including GLFW
add_subdirectory(libraries/glfw)
include_directories(libraries/glfw/include)


find_package(Freetype REQUIRED)

# Including GLAD
include_directories(libraries/glad/include)

include_directories(libraries/bundle)

set(SOURCE_FILES main.cpp libraries/glad/src/glad.c
        src/Rendering/Window.cpp
        src/Rendering/Window.h
        src/Utility/InputManager.cpp
        src/Utility/InputManager.h
        src/Utility/InputManager.cpp
        src/Utility/InputManager.h
        src/Utility/Keybind.cpp
        src/Utility/Keybind.h
        src/Utility/Input.cpp
        src/Utility/Input.h
        src/Rendering/RenderUtil.h
        src/Rendering/RenderUtil.cpp
        src/Rendering/RenderUtil.h
        src/Rendering/TriangleMesh.cpp
        src/Rendering/TriangleMesh.h
        src/Rendering/Vertices.h
        src/Rendering/Shader.cpp
        src/Rendering/Shader.h
        src/Rendering/Texture.h
        src/Rendering/Texture.cpp
        src/Scene/Camera.cpp
        src/Scene/Camera.h
        src/Game/Location/Location.cpp
        src/Game/Location/Location.h
        src/Game/Location/LocationGenerator.cpp
        src/Game/Location/LocationGenerator.h
        src/Utility/PerlinNoise.h
        src/Utility/PerlinNoise.cpp
        src/Rendering/LocationRenderer.cpp
        src/Rendering/LocationRenderer.h
        src/Rendering/TriangleMeshTEX.cpp
        src/Rendering/TriangleMeshTEX.h
        src/Scene/QuadTree.cpp
        src/Scene/QuadTree.h
        src/Utility/TextureUtil.cpp
        src/Utility/TextureUtil.h
        src/Utility/ResourceLoader.cpp
        src/Utility/ResourceLoader.h
        src/Utility/MathUtil.h
        src/Rendering/OpenGLStructures.h
        src/Scene/Terrain.cpp
        src/Scene/Terrain.h
        src/Rendering/Model.h
        src/Rendering/Model.cpp
        src/Rendering/ModelRenderer.cpp
        src/Rendering/ModelRenderer.h
        src/Rendering/TerrainRenderer.cpp
        src/Rendering/TerrainRenderer.h
        src/Game/Location/LocationState.h
        src/Game/Base/GameState.cpp
        src/Game/Location/PlayerController.cpp
        src/Game/Location/LocationState.cpp
        src/Game/Systems/EntityPhysicsSystem.cpp
        src/Game/Systems/EntityPhysicsSystem.h
        src/Game/Systems/EntityPhysicsSystem.h
        src/Game/Systems/LocationSystem.h
        src/Rendering/MassModelRenderer.cpp
        src/Rendering/MassModelRenderer.h
        src/Rendering/Buffer.h
        src/Rendering/Texture3D.cpp
        src/Rendering/Texture3D.h
        src/GUI/FontManager.h
        src/GUI/GUIRenderer.cpp
        src/GUI/GUIRenderer.h
        src/Utility/JsonReader.cpp
        src/Utility/JsonReader.h
        src/Game/Entity/EntityComponents.h
        src/Game/Entity/EntityLoader.cpp
        src/Game/Entity/EntityLoader.h
        src/Rendering/Studio/ModelCapture.cpp
        src/Rendering/Studio/ModelCapture.h
        src/Utility/Resource.h
        src/Game/Entity/EntityComponentStorage.h)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

# Linking GLFW and OGL
target_link_libraries(${CMAKE_PROJECT_NAME} glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARY})

target_link_libraries(${CMAKE_PROJECT_NAME} ${FREETYPE_LIBRARIES})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${FREETYPE_INCLUDE_DIRS})

