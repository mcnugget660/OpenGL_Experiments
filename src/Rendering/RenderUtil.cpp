#include <string>
#include "RenderUtil.h"

#include "glad/glad.h" // Must be before GLFW

void RenderUtil::clearBuffer() {
    glClearColor(0.2f, 0.1f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
