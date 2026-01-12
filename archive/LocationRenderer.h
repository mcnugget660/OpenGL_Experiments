#ifndef FINALDAYONEARTH_LOCATIONRENDERER_H
#define FINALDAYONEARTH_LOCATIONRENDERER_H

#include "TerrainRenderer.h"
#include "ModelRenderer.h"
#include "shapeRenderer.h"
#include "Buffer.h"

class Camera;

struct ModelInstanceData{
    int startIndex;
    int number;
};

class LocationRenderer {
public:
    LocationRenderer();

    ~LocationRenderer();

    void setLocation(Location *location);

    void renderLocation(Camera &camera, Camera &viewCamera, double time);

private:
    ShapeRenderer shapeRenderer;
    Shader basicShader;

    Location *location;
    TerrainRenderer terrainRenderer;

    Shader modelShader;

    // Model Renderers
    std::vector<ModelRenderer> staticModelRenderers;
    std::vector<ModelInstanceData> staticModelRendererData;

    std::vector<ModelRenderer> dynamicModelRenderers;
    std::vector<ModelInstanceData> dynamicModelRendererData;

    // Entity Data
    std::vector<glm::mat4x4> entityModelMatrices;
    
    const uint MAX_ENTITIES = -1;

    Buffer<glm::mat4> entityBuffer;
    
    // Landscape Data
    std::vector<unsigned int> landscapeBuffers;

    void setUp();

    void updateEntityBuffer();

    int canFitInBuffer(int number);

    void resizeInstanceBuffer(int size);

};


#endif //FINALDAYONEARTH_LOCATIONRENDERER_H
