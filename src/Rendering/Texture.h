#ifndef FINALDAYONEARTH_TEXTURE_H
#define FINALDAYONEARTH_TEXTURE_H

#include <string>
#include "glad/glad.h"
#include "../Utility/ResourceLoader.h"

class Texture {
    friend class ResourceLoader;
public:
    Texture(std::string name, std::string path, bool flipped);

    explicit Texture(unsigned int ID);

    ~Texture();

    void bind(int location) const;

    GLuint64 makeResident();

    GLuint64 getHandle();

private:
    unsigned int ID = -1;
    GLuint64 handle = -1;

    void unload();
};


#endif //FINALDAYONEARTH_TEXTURE_H
