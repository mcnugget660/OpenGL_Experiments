#ifndef FINALDAYONEARTH_MASSMODELRENDERER_H
#define FINALDAYONEARTH_MASSMODELRENDERER_H

#include <unordered_map>
#include <string>
#include <vector>
#include <glm/fwd.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include "Model.h"
#include "../Scene/Camera.h"
#include "Buffer.h"
#include "OpenGLStructures.h"
#include "Texture3D.h"

struct ModelRenderData{
    unsigned int firstMesh;
    unsigned int meshCount;
    ModelRenderData(unsigned int firstMesh, unsigned int meshCount) : firstMesh(firstMesh), meshCount(meshCount){}

};

struct MeshRenderData {
    unsigned int vertexStart;
    unsigned int vertexCount;
    unsigned int elementStart;
    unsigned int elementCount;
    unsigned int textureStart;
    unsigned int textureCount;
};

struct InstanceData{
    glm::mat4 modelMatrix;
    float radius;
    uint32_t modelIndex;
    uint32_t _padding[2];

    InstanceData(glm::mat4 modelMatrix, float radius, uint32_t modelIndex) : modelMatrix(modelMatrix), radius(radius), modelIndex(modelIndex){}
};

struct GPUModelInstanceData{
    unsigned int count;
    unsigned int instanceOffset;
};

class MassModelRenderer {

public:
    MassModelRenderer(int TextureSize = 512);

    ~MassModelRenderer();

    void setInstanceData(std::vector<std::vector<InstanceData>> instanceData, unsigned int modelOffset);

    void cullInstances(Camera& camera);

    void passInstances();

    // Renders models according to ModelInstanceData
    void renderModels(int modelOffset, int totalModels);

    void renderMesh(int model, int mesh);

    int addModel(std::string name);

    // Must call after adding models
    void configureModels();

private:
    Buffer<Vertex> LargeVertexBuffer;
    Buffer<unsigned int> LargeElementsBuffer;

    Buffer<InstanceData> InstanceInBuffer;
    Buffer<InstanceData> InstanceOutBuffer;
    Buffer<DrawElementsIndirectCommand> IndirectCommandBuffer;
    Buffer<unsigned int> MeshToModelBuffer;

    Buffer<GPUModelInstanceData> ModelInstanceDataBuffer;
    std::vector<GPUModelInstanceData> modelInstanceData;
    std::vector<GPUModelInstanceData> modelInstanceFilter;

    unsigned int VAO;
    std::vector<std::string> textures;
    Texture3D *textureArray;

    std::vector<MeshRenderData> meshRenderData;
    std::vector<ModelRenderData> modelRenderData;
    std::unordered_map<std::string, unsigned int> modelID;

    Shader* culler;
    Shader* setMeshCounts;

    int TextureSize = 512;

    void frustumCull(Camera &camera);

    void loadVAO();

};

#endif //FINALDAYONEARTH_MASSMODELRENDERER_H
