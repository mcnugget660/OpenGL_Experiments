#ifndef LEARNOPENGL_CLION_WINDOW_H
#define LEARNOPENGL_CLION_WINDOW_H

class GLFWwindow;

class Window {

public:
    static void start();

    static bool buildDefault();

    static bool build(int width, int height);

    static void terminate();

    static bool shouldClose();

    static void swapBuffer();

    static void setClosed();

    static void hideMouse();

    static void showMouse();

    static void toggleMouse();

    static bool wasResized();

    static void setResized();

    static int getWidth();

    static int getHeight();

    static GLFWwindow *getWindow();

private:

    static GLFWwindow *window;

    static bool resized;

    static int width;
    static int height;

    void static debugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                             const GLchar *message, const void *userParam);

    static void glfw_error_callback(int error, const char *description);
};


#endif //LEARNOPENGL_CLION_WINDOW_H
