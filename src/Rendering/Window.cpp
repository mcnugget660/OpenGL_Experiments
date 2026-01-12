#include <iostream>

#include "glad/glad.h" // Must be before GLFW
#include "GLFW/glfw3.h"

#include "Window.h"

GLFWwindow *Window::window = nullptr;
int Window::width = 500;
int Window::height = 500;
bool Window::resized = false;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void Window::start() {
    glfwInit();
}

bool Window::buildDefault() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    return build(500, 500);
}

bool Window::build(int width, int height) {
    window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwSetErrorCallback(glfw_error_callback);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "OpenGL version: " << version << std::endl;
    if (GLAD_GL_ARB_multi_draw_indirect) {
        std::cout << "GL_ARB_multi_draw_indirect is supported!" << std::endl;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_STENCIL_TEST);


    //Anti Aliasing
    glEnable(GL_MULTISAMPLE);



    //Debug
    auto d = [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                const GLchar *message, const void *userParam) {
        debugMessage(source, type, id, severity, length, message, userParam);
    };
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(d, NULL);

    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, NULL, GL_TRUE);

    return true;
}

void Window::debugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                          const GLchar *message, const void *userParam) {
    std::cout << message << std::endl;
}

void Window::glfw_error_callback(int error, const char *description) {
    std::cerr << "GLFW Error: " << description << std::endl;
}

void Window::terminate() {
    glfwTerminate();
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(window);
}

void Window::setClosed() {
    glfwSetWindowShouldClose(window, true);
}

void Window::swapBuffer() {
    glfwSwapBuffers(window);
}

void Window::hideMouse() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::showMouse() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::toggleMouse() {
    glfwSetInputMode(window, GLFW_CURSOR, glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

GLFWwindow *Window::getWindow() {
    return window;
}

bool Window::wasResized() {
    if (resized) {
        glfwGetWindowSize(window, &width, &height);
        resized = false;
        return true;
    }
    return false;
}

void Window::setResized() {
    resized = true;
}

int Window::getWidth() {
    return width;
}

int Window::getHeight() {
    return height;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    Window::setResized();
}