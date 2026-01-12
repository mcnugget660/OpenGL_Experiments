#include <iostream>
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glad/glad.h"

Texture::Texture(std::string name, std::string path, bool flipped) {
    std::string str = path + "/" + name;

    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);

    stbi_set_flip_vertically_on_load(flipped);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(str.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        auto format = nrChannels == 3 ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture\n";
    }
    stbi_image_free(data);
}

Texture::Texture(unsigned int ID) : ID(ID){
}

void Texture::bind(int location) const {
    if(location > 15)
        std::cout << "TEXTURE Bound to non-existent texture unit!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    glActiveTexture(GL_TEXTURE0 + location);
    glBindTexture(GL_TEXTURE_2D, ID);
}

Texture::~Texture() {
    unload();
}

GLuint64 Texture::makeResident() {
    handle = glGetTextureHandleARB(ID);
    glMakeTextureHandleResidentARB(handle);
    return handle;
}

GLuint64 Texture::getHandle() {
    assert(handle != -1);
    return handle;
}

void Texture::unload() {
    if(ID != -1)
        glDeleteTextures(1, &ID);
    if(handle != -1)
        glMakeTextureHandleNonResidentARB(handle);
}
