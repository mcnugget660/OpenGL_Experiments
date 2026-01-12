#ifndef FINALDAYONEARTH_TEXTURE3D_H
#define FINALDAYONEARTH_TEXTURE3D_H

#include <string>
#include "glad/glad.h"
#include <vector>

class Texture3D {
public:
    Texture3D(std::vector<std::string> paths, int textureSize);

    explicit Texture3D(unsigned int ID);

    ~Texture3D();

    void bind(int location) const;

    GLuint64 makeResident();

    GLuint64 getHandle();

private:
    unsigned int ID = -1;
    GLuint64 handle = -1;
};

#endif //FINALDAYONEARTH_TEXTURE3D_H
