#ifndef FINALDAYONEARTH_TEXTUREUTIL_H
#define FINALDAYONEARTH_TEXTUREUTIL_H


#include <vector>
#include "../Rendering/Texture.h"

class TextureUtil {
public:
    static Texture *floatArray2DToTexture(float *array, int size, bool nearest = true, bool repeat = false);

    static Texture *colorArray2DToTexture(unsigned char *array, int size);

    static Texture *colorArray2DToTexture(std::vector<unsigned char> array, int size);
};


#endif //FINALDAYONEARTH_TEXTUREUTIL_H
