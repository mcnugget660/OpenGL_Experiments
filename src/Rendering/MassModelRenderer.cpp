#include <iostream>
#include "MassModelRenderer.h"
#include "glad/glad.h"
#include "../Utility/ResourceLoader.h"

MassModelRenderer::MassModelRenderer(int TextureSize) : TextureSize(TextureSize) {
    loadVAO();
    culler = ResourceLoader::getShader("cull");
    setMeshCounts = ResourceLoader::getShader("setMeshCounts");
}
                                                                                                                        // Meshes are lined up in sequence
int MassModelRenderer::addModel(std::string name) {
    if(!modelID.count(name)) {
        Model *model = ResourceLoader::getModel(name);

        modelRenderData.emplace_back(static_cast<unsigned int>(meshRenderData.size()), static_cast<unsigned int>(model->meshes.size()));
        unsigned int meshToModel = modelID[name] = modelID.size();
        modelInstanceData.emplace_back(GPUModelInstanceData{0, 0});
        modelInstanceFilter.emplace_back(GPUModelInstanceData{0, 0});

        std::vector<Vertex> allVertices;
        std::vector<unsigned int> allElements;
        std::vector<std::string> diffTx;
        std::vector<std::string> normTx;
        std::vector<std::string> specTx;

        std::vector<unsigned int> meshID;
        for (ModelMesh &mesh: model->meshes) {
            bool diff = mesh.diffuse != -1;
            bool norm = mesh.normal != -1;
            bool spec = mesh.specular != -1;
            meshRenderData.emplace_back(MeshRenderData{static_cast<unsigned int>(LargeVertexBuffer.dataCount + allVertices.size()), static_cast<unsigned int>(mesh.vertices.size()),
                                        static_cast<unsigned int>(LargeElementsBuffer.dataCount + allElements.size()), static_cast<unsigned int>(mesh.indices.size()), static_cast<unsigned int>(textures.size()), static_cast<unsigned int>(diff + norm + spec)});

            allVertices.insert(allVertices.end(), mesh.vertices.begin(), mesh.vertices.end());
            allElements.insert(allElements.end(), mesh.indices.begin(), mesh.indices.end());

            if (diff)
                diffTx.push_back(model->texturePath + "/" + std::to_string(mesh.diffuse) + ".png");
            if (norm)
                normTx.push_back(model->texturePath + "/" + std::to_string(mesh.normal) + ".png");
            if (spec)
                specTx.push_back(model->texturePath + "/" + std::to_string(mesh.specular) + ".png");

            meshID.push_back(meshToModel);
        }

        MeshToModelBuffer.addData(meshID);

        LargeVertexBuffer.addData(allVertices);
        LargeElementsBuffer.addData(allElements);
        textures.insert(textures.end(), diffTx.begin(), diffTx.end());
        return meshToModel;
    }
    std::cout << "Redundant addModel Call\n";
    return -1;
}

// Will remove all instances after offset
void MassModelRenderer::setInstanceData(std::vector<std::vector<InstanceData>> instanceData, unsigned int modelOffset) {
    if(modelOffset + instanceData.size() > modelRenderData.size()) {
        std::cout << "Error : setInstanceData for no-existent models\n";
        return;
    }

    int index = 0;
    int offset = modelOffset == 0 ? 0 : modelInstanceData.at(modelOffset - 1).instanceOffset + modelInstanceData.at(modelOffset - 1).count;
    int initialOffset = offset;
    std::vector<InstanceData> data;
    for(int i = modelOffset; i < modelOffset + instanceData.size(); i++) {
        std::vector<InstanceData> &list = instanceData.at(index++);
       modelInstanceData.at(i).instanceOffset = offset;
       offset += modelInstanceData.at(i).count = list.size();
       data.insert(data.end(), list.begin(), list.end());
    }
    for(int i = modelOffset + instanceData.size(); i < modelInstanceData.size(); i++) {
        modelInstanceData.at(i).instanceOffset = offset;
        modelInstanceData.at(i).count = 0;
    }
    for(int i = modelOffset; i < modelInstanceData.size(); i++)
        modelInstanceFilter[i] = modelInstanceData.at(i);

    InstanceInBuffer.setData(data, initialOffset);
    InstanceOutBuffer.setBufferSize(InstanceInBuffer.dataCount, 0);
}

void MassModelRenderer::passInstances() { // will not work yet
    ModelInstanceDataBuffer.setDataExactSize(modelInstanceData);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ModelInstanceDataBuffer.buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, MeshToModelBuffer.buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, IndirectCommandBuffer.buffer);

    // Set Mesh Indirect Commands
    setMeshCounts->use();

    glDispatchCompute(1, 1, meshRenderData.size());

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void MassModelRenderer::cullInstances(Camera &camera) {
        // Set Commands
        std::vector<DrawElementsIndirectCommand> commands(meshRenderData.size());
        for(int i = 0; i < meshRenderData.size(); i++)
            commands[i] = {0,0,0,0,0};

        for(ModelRenderData renderData : modelRenderData) {
            for(int i = 0; i < renderData.meshCount; i++) {
                MeshRenderData meshData = meshRenderData.at(renderData.firstMesh + i);
                commands[renderData.firstMesh + i] = {meshData.elementCount, 0, meshData.elementStart, static_cast<int>(meshData.vertexStart), 0};
            }
        }
        IndirectCommandBuffer.setData(commands);

        for(auto & instanceFilter : modelInstanceFilter)
            instanceFilter.count = 0;

        ModelInstanceDataBuffer.setDataExactSize(modelInstanceFilter);

        frustumCull(camera);
}

void MassModelRenderer::frustumCull(Camera &camera) {
    // Bind Data
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, InstanceInBuffer.buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, InstanceOutBuffer.buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ModelInstanceDataBuffer.buffer);

    // Frustum Culling Compute Shader
    culler->use();

    Frustum frustum = camera.buildFrustum();

    for (int i = 0; i < 6; i++) {
        culler->setVec3("planes[" + std::to_string(i) + "].normal", (*(((Plane *) &frustum) + i)).normal);
        culler->setFloat("planes[" + std::to_string(i) + "].distance", (*(((Plane *) &frustum) + i)).distance);
    }

    culler->setInt("inputNumber", InstanceInBuffer.dataCount);
    glDispatchCompute(1, 1, (int) std::ceil(static_cast<float>(InstanceInBuffer.dataCount) / 32.0));

    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ModelInstanceDataBuffer.buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, MeshToModelBuffer.buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, IndirectCommandBuffer.buffer);

    // Set Mesh Indirect Commands
    setMeshCounts->use();

    glDispatchCompute(1, 1, meshRenderData.size());

    glMemoryBarrier(GL_ALL_BARRIER_BITS);

}

void MassModelRenderer::renderModels(int modelOffset, int totalModels) {
    if(totalModels + modelOffset > modelRenderData.size()) {
        std::cout << "Error : attempt to render non-existent model(s)\n";
        return;
    }
    unsigned int offset = modelRenderData.at(modelOffset).firstMesh, totalMeshes = 0;
    for(int i = 0; i < totalModels; i++) {
        totalMeshes += modelRenderData.at(modelOffset + i).meshCount;
    }
    glBindVertexArray(VAO);
    textureArray->bind(0);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IndirectCommandBuffer.buffer);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, reinterpret_cast<const void *>(sizeof(DrawElementsIndirectCommand) * offset), totalMeshes, 0);
    glBindVertexArray(0);
}

void MassModelRenderer::renderMesh(int model, int mesh) {
    int meshNumber = modelRenderData.at(model).firstMesh + mesh;
    glBindVertexArray(VAO);
    textureArray->bind(0);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IndirectCommandBuffer.buffer);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, reinterpret_cast<const void *>(sizeof(DrawElementsIndirectCommand) * meshNumber), 1, 0);
    glBindVertexArray(0);
}

MassModelRenderer::~MassModelRenderer() {
  delete(textureArray);
}

void MassModelRenderer::loadVAO() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, LargeVertexBuffer.buffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LargeElementsBuffer.buffer);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);

    // Texture Coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (sizeof(float) * 3));

    // Model Matrix
    glBindBuffer(GL_ARRAY_BUFFER, InstanceOutBuffer.buffer);

    std::size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glVertexAttribDivisor(3, 1);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glVertexAttribDivisor(4, 1);

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
    glVertexAttribDivisor(5, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MassModelRenderer::configureModels() {
    textureArray = new Texture3D(textures, TextureSize);
}
