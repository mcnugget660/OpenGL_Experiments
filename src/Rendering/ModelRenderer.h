#ifndef FINALDAYONEARTH_MODELRENDERER_H
#define FINALDAYONEARTH_MODELRENDERER_H

#include <glm/ext/matrix_float4x4.hpp>
#include "Texture.h"

#include "Model.h"

// On GPU

class Model;
class ModelRenderer {
public:

    explicit ModelRenderer(std::string name, unsigned int instanceData);

    ~ModelRenderer();

    void render(int startIndex, int number);

    void load();

    void unload();
private:
    Model *model = nullptr;

    std::vector<unsigned int> VAOS;
    std::vector<unsigned int> VBOS;
    std::vector<unsigned int> EBOS;

    std::vector<Texture *> textures;

    bool loaded = false;

    std::string modelName;

    unsigned int instanceData;

};


#endif //FINALDAYONEARTH_MODELRENDERER_H
