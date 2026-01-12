#ifndef FINALDAYONEARTH_MODELCAPTURE_H
#define FINALDAYONEARTH_MODELCAPTURE_H


#include "../Shader.h"
#include "../Texture.h"
#include "../../Scene/Camera.h"

class ModelCapture {
public:
    static Texture* drawModelToTexture(std::string model_name);
private:
    static Shader* shader;
    static unsigned int framebuffer;
    static Camera camera;
};


#endif //FINALDAYONEARTH_MODELCAPTURE_H
