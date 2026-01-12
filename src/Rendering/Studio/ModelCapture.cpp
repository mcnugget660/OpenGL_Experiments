#include "ModelCapture.h"
#include "../../Utility/ResourceLoader.h"
#include "../ModelRenderer.h"
#include "../Buffer.h"

Shader* ModelCapture::shader = nullptr;
unsigned int ModelCapture::framebuffer = -1;
Camera ModelCapture::camera{0.0f, 0.0f, -5.0f, 16.0f/9.0f};

Texture *ModelCapture::drawModelToTexture(std::string model_name) {
    if(shader == nullptr)
        shader = ResourceLoader::getShader("ModelShader");
    if(framebuffer == -1)
        glGenFramebuffers(1, &framebuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    Buffer<glm::mat4> matrix(1);
    std::vector<glm::mat4> mm;
    mm.push_back(glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));
    matrix.setDataExactSize(mm);
    ModelRenderer mr(model_name, matrix.buffer);


    glm::mat4 view = camera.getCameraMatrix();
    glm::mat4 proj = camera.getProjectionMatrix();

    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", proj);

    mr.render(0, 1);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return nullptr;
}
