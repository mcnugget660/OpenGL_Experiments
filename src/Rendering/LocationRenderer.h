#ifndef FINALDAYONEARTH_LOCATIONRENDERER_H
#define FINALDAYONEARTH_LOCATIONRENDERER_H

#include "TerrainRenderer.h"
#include "ModelRenderer.h"
#include "shapeRenderer.h"
#include "Buffer.h"
#include "MassModelRenderer.h"

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
    Shader* basicShader;

    Location *location;
    TerrainRenderer terrainRenderer;

    Shader* modelShader;

    // Entity Data
    const uint MAX_RENDERING_ENTITIES = 100;
    
    // Model Rendering
    MassModelRenderer massModelRenderer;
    uint staticModels = 0;
    uint dynamicModels = 0;
    uint staticMeshes = 0;

    void setUp();

    void updateEntityBuffer();

};


#endif //FINALDAYONEARTH_LOCATIONRENDERER_H
