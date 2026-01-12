#include "LocationRenderer.h"
#include "../Scene/Camera.h"
#include "../Utility/ResourceLoader.h"

#define MAX_ENTITIES 1000

LocationRenderer::LocationRenderer() {
    modelShader = ResourceLoader::getShader("MassModelShader");
    basicShader = ResourceLoader::getShader("basic");
    setUp();
}

void LocationRenderer::setLocation(Location *location){
    LocationRenderer::location = location;

    staticModels = location->staticModels.size();
    dynamicModels = location->dynamicModels.size();
    staticMeshes = 0;

    std::vector<std::vector<InstanceData>> instances;
    // Load landscape models
    for(int i = 0; i < location->staticModels.size(); i++){
        std::vector<InstanceData> inst;
        for(glm::vec2 &loc : location->resourceLocations[i]){
            glm::mat4 mat(1.0f);
           inst.emplace_back(glm::translate(mat, glm::vec3(loc.x, location->heightMapAt(loc.x, loc.y), loc.y)), 3, i);
       }
        instances.push_back(inst);

        massModelRenderer.addModel(location->staticModels[i]);
        staticMeshes += ResourceLoader::getModel(location->staticModels[i])->meshes.size();
    }
    massModelRenderer.setInstanceData(instances, 0);

    // Load entity models
    for(int i = 0; i < location->dynamicModels.size(); i++){
        massModelRenderer.addModel(location->dynamicModels[i]);
    }

    massModelRenderer.configureModels();
}

void LocationRenderer::renderLocation(Camera &camera, Camera &viewCamera, double time) {
    std::vector<TerrainNode *> nodes;
    location->quadTree.getNodes(viewCamera, nodes);

    // Render terrain
    terrainRenderer.renderLocation(location, viewCamera, viewCamera, time, nodes); // Must be first so blended objects blend with the proper background

    updateEntityBuffer();
    massModelRenderer.cullInstances(camera);

    glDisable(GL_CULL_FACE);
    modelShader->use();
    glm::mat4 view = viewCamera.getCameraMatrix();
    glm::mat4 proj = viewCamera.getProjectionMatrix();
    modelShader->setMat4("view", view);
    modelShader->setMat4("projection", proj);
    modelShader->setInt("diffuse", 0);
    modelShader->setInt("DrawIDOffset", 0);

    // Render static models
    massModelRenderer.renderModels(0, staticModels);

    // Smooth Edges -- Work to improve this smoothness
    /*modelShader->setBool("alphaDiscard", false);
    glEnable( GL_BLEND );
    glDepthMask( GL_FALSE );
    massModelRenderer.renderMesh(0, 0);
    modelShader->setBool("alphaDiscard", true);
    glDisable( GL_BLEND );
    glDepthMask( GL_TRUE );*/

    // Grass
    terrainRenderer.renderGrass(location, camera, viewCamera, time, nodes);


    glEnable(GL_CULL_FACE);

    if(dynamicModels > 0) {
        modelShader->use();
        modelShader->setInt("DrawIDOffset", staticMeshes);

        // Render dynamic models
        massModelRenderer.renderModels(staticModels, dynamicModels);

        basicShader->use();
        basicShader->setMat4("view", view);
        basicShader->setMat4("projection", proj);


        /*for (auto pos: location->entityPositions)
            shapeRenderer.fillRectangularPrism({10, 10, 10}, {pos.second.x, location->heightMapAt(pos.second.x, pos.second.y), pos.second.y},
                                               *basicShader);
        */

    }

    Shader * sdfShader = ResourceLoader::getShader("3d");
    sdfShader->use();
    sdfShader->setMat4("view", view);
    sdfShader->setMat4("projection", proj);
    glm::vec3 fw = viewCamera.getLocation();

    double a = (2 * M_PI / 1440);

    glm::vec3 sunPos = glm::vec3(std::cos(a * (time - 360.0)) * 1000, std::sin(a * (time - 360.0)) * 1000, 0.0f);
    glm::vec3 moonPos = glm::vec3(-std::cos(a * (time - 360.0)), -std::sin(a * (time - 360.0)), 0.0f);

    // Eventually use a realistic orbit
    sdfShader->setVec3("sunPos", sunPos);
    sdfShader->setVec3("moonPos", moonPos);

    sdfShader->setVec3("cameraPos", fw);
    glm::vec3 color = glm::vec3(255, 20, 40);
    glm::vec3 scale = glm::vec3(1);

    for(auto& [shape, ids] : location->sdfInstances) {
        for(EntityID id : ids)
            shapeRenderer.fillRectangularPrism(scale,glm::vec3(location->ECS.entityPositions[id].x, .5 + location->heightMapAt(location->ECS.entityPositions[id].x, location->ECS.entityPositions[id].y), location->ECS.entityPositions[id].y), *sdfShader, color);
    }

    //basicShader->use();
    //basicShader->setMat4("view", view);
   // basicShader->setMat4("projection", proj);
    //basicShader->setMat4("model", glm::mat4(1));
   // camera.drawFrustum(shapeRenderer, *basicShader);
    //shapeRenderer.drawRectangularPrism(glm::vec3(1),b, *basicShader, glm::vec3(255, 0, 0));
}

void LocationRenderer::updateEntityBuffer() {
    // Fill model matrices and update MassModelRenderer data
    std::vector<std::vector<InstanceData>> instances;
    uint currentInstances = 0;
    for(auto& [modelID, idVec] : location->entityModelInstances) {
        int inBuffer = std::min((uint) idVec.size(), std::max((uint) 0, (uint) (MAX_ENTITIES - currentInstances - idVec.size())));
        if(inBuffer == 0)
            break;

        std::vector<InstanceData> inst;

        for(unsigned int& id : idVec){
            EntityMovementC &data = location->ECS.entityMovement[id];
            glm::mat4 rotationMatrix = glm::lookAt(glm::cross(glm::vec3{0.0f,1.0f,0.0f}, glm::vec3{data.direction.x, 0.0f, data.direction.y}), {0.0f,1.0f,0.0f}, {data.direction.x, 0.0f, data.direction.y});

            glm::mat4 mat4(1.0f);
            glm::vec2& pos = location->ECS.entityPositions[id];
            mat4 = glm::translate(mat4, glm::vec3(pos.x, location->heightMapAt(pos.x, pos.y), pos.y));

            inst.emplace_back(mat4, 2, staticModels + modelID);
            if(inst.size() == inBuffer)
                break;
        }

        instances.push_back(inst);

    }

    massModelRenderer.setInstanceData(instances, staticModels);
}


void LocationRenderer::setUp() {
   // Nothing yet
}

LocationRenderer::~LocationRenderer() {
}