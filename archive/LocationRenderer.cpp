#include "LocationRenderer.h"
#include "../Scene/Camera.h"
#include "../Utility/ResourceLoader.h"

#define MAX_ENTITIES 1000

LocationRenderer::LocationRenderer() : entityBuffer(MAX_ENTITIES), modelShader("/home/conrad/c++Projects/Final Day On Earth/resources/shaders/ModelShader.vert", "/home/conrad/c++Projects/Final Day On Earth/resources/shaders/ModelShader.frag"),
                                       basicShader("/home/conrad/c++Projects/Final Day On Earth/resources/shaders/basic.vert", "/home/conrad/c++Projects/Final Day On Earth/resources/shaders/basic.frag"){
    setUp();
}

void LocationRenderer::setLocation(Location *location){
    LocationRenderer::location = location;

    staticModelRenderers.clear();
    staticModelRendererData.clear();

    dynamicModelRenderers.clear();
    dynamicModelRendererData.clear();

    // Load landscape models
    for(int i = 0; i < location->staticModels.size(); i++){
        std::vector<glm::mat4> modelMatrices;
        for(glm::vec2 &loc : location->resourceLocations[i]){
            glm::mat4 mat(1.0f);
            modelMatrices.push_back(glm::translate(mat, glm::vec3(loc.x, location->heightMapAt(loc.x, loc.y), loc.y)));
        }

        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * modelMatrices.size(), modelMatrices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        landscapeBuffers.push_back(buffer);
        staticModelRenderers.emplace_back(location->staticModels[i], buffer);
        staticModelRendererData.emplace_back(0, modelMatrices.size());
    }

    // Load entity models
    for(int i = 0; i < location->dynamicModels.size(); i++){
        dynamicModelRenderers.emplace_back(location->dynamicModels[i], entityBuffer.buffer);
        dynamicModelRendererData.emplace_back(0, 0);
    }

    // Load onto GPU
    for(ModelRenderer &renderer : staticModelRenderers){
        renderer.load();
    }

    for(ModelRenderer &renderer : dynamicModelRenderers){
        renderer.load();
    }

}

void LocationRenderer::renderLocation(Camera &camera, Camera &viewCamera, double time) {
    std::vector<TerrainNode *> nodes;
    location->quadTree.getNodes(viewCamera, nodes);

    // Render terrain
    terrainRenderer.renderLocation(location, viewCamera, viewCamera, time, nodes); // Must be first so blended objects blend with the proper background


    glDisable(GL_CULL_FACE);

    modelShader.use();
    glm::mat4 view = camera.getCameraMatrix();
    glm::mat4 proj = camera.getProjectionMatrix();
    modelShader.setMat4("view", view);
    modelShader.setMat4("projection", proj);
    modelShader.setInt("diffuse", 0);
    modelShader.setInt("specular", 1);
    modelShader.setInt("normal", 2);


    // Render static models
    for(int i = 0; i < staticModelRenderers.size(); i++){
        ModelInstanceData &data = staticModelRendererData[i];
        staticModelRenderers[i].render(data.startIndex, data.number);
    }

    terrainRenderer.renderGrass(location, viewCamera, viewCamera, time, nodes);
    glEnable(GL_CULL_FACE);

    modelShader.use();

    updateEntityBuffer();

    // Render dynamic models
    for(int i = 0; i < dynamicModelRenderers.size(); i++){
        ModelInstanceData &data = dynamicModelRendererData[i];
        dynamicModelRenderers[i].render(data.startIndex, data.number);
    }

    basicShader.use();
    basicShader.setMat4("view", viewCamera.getCameraMatrix());
    basicShader.setMat4("projection", viewCamera.getProjectionMatrix());


    for(auto pos : location->entityPositions)
        shapeRenderer.fillRectangularPrism({10, 10, 10}, {pos.second.x, location->heightMapAt(pos.second.x, pos.second.y), pos.second.y}, basicShader);


}

void LocationRenderer::updateEntityBuffer() {
    // Fill model matrices and update modelRenderer data
    entityModelMatrices.clear();
    for(auto& [modelID, idVec] : location->entityModelInstances) {
        int inBuffer = std::min((uint) idVec.size(), std::max((uint) 0, (uint) (entityBuffer.maxSize - entityModelMatrices.size() - idVec.size())));
        if(inBuffer == 0)
            break;

        dynamicModelRendererData[modelID] = ModelInstanceData(entityModelMatrices.size(), inBuffer);

        for(int& id : idVec){
            EntityMovementData &data = location->entityMovement[id];
            glm::mat4 rotationMatrix = glm::lookAt(glm::cross(glm::vec3{0.0f,1.0f,0.0f}, glm::vec3{data.direction.x, 0.0f, data.direction.y}), {0.0f,1.0f,0.0f}, {data.direction.x, 0.0f, data.direction.y});

            glm::mat4 mat4(1.0f);
            glm::vec2& pos = location->entityPositions[id];
            mat4 = glm::translate(mat4, glm::vec3(pos.x, location->heightMapAt(pos.x, pos.y), pos.y));

            entityModelMatrices.push_back(mat4);

        }

    }

    entityBuffer.setData(entityModelMatrices);
}


void LocationRenderer::setUp() {
    // Nothing yet
}

LocationRenderer::~LocationRenderer() {
    glDeleteBuffers(landscapeBuffers.size(), landscapeBuffers.data());
}