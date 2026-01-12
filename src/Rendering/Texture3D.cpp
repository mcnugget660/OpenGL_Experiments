#include <iostream>
#include <cmath>
#include "Texture3D.h"

#include "stb_image.h"

#include "glad/glad.h"

Texture3D::Texture3D(std::vector<std::string> paths, int textureSize) {
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D_ARRAY, ID);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, log2(textureSize) + 1, GL_RGBA8, textureSize, textureSize, paths.size());

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.0f);

    for(int i = 0; i < paths.size(); i++) {
        int width, height, nrChannels;
        unsigned char *data = stbi_load(paths.at(i).c_str(), &width, &height, &nrChannels, 4);
        if (data) {
            if (nrChannels != 4)
                std::cout << "Error : 3DTexture path points to non RGBA image " + paths.at(i) << std::endl;
            if (width != textureSize || height != textureSize)
                std::cout << "Error : 3DTexture path points to non " + std::to_string(textureSize) + "x" + std::to_string(textureSize) + " image " + paths.at(i)
                          << std::endl;
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, textureSize, textureSize, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        } else {
            std::cout << "Failed to load texture " + paths.at(i) + "\n";
        }
        stbi_image_free(data);
    }

    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}

Texture3D::Texture3D(unsigned int ID) : ID(ID){
}

Texture3D::~Texture3D() {
    if(ID != -1)
        glDeleteTextures(1, &ID);
    if(handle != -1)
        glMakeTextureHandleNonResidentARB(handle);
}
void Texture3D::bind(int location) const {
    if(location > 15)
        std::cout << "TEXTURE Bound to non-existent texture unit!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    glActiveTexture(GL_TEXTURE0 + location);
    glBindTexture(GL_TEXTURE_2D_ARRAY, ID);
}


GLuint64 Texture3D::makeResident() {
    handle = glGetTextureHandleARB(ID);
    glMakeTextureHandleResidentARB(handle);
    return handle;
}

GLuint64 Texture3D::getHandle() {
    return handle;
}
