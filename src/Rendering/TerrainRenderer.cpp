#include "TerrainRenderer.h"

#include <iostream>
#include <glm/gtc/type_ptr.inl>
#include "../Scene/Camera.h"
#include "../Utility/ResourceLoader.h"
#include "OpenGLStructures.h"

std::vector<glm::vec2> TerrainRenderer::squareVertices = {
        {1.0f, 1.0f},
        {0.0f, 1.0f},
        {1.0f, 0.0f},
        {0.0f, 0.0f}
};

TerrainRenderer::TerrainRenderer() :  zeros(maxGrassTiles, 0) {
    terrainShader = ResourceLoader::getShader("Terrain");
    grassShader= ResourceLoader::getShader("Grass");
    grassNormalShader= ResourceLoader::getShader("normals");
    basicShader= ResourceLoader::getShader("basic");
    occludeBoundingBoxShader= ResourceLoader::getShader("occlude");
    genBlades= ResourceLoader::getShader("grass");
    occlusionCulling= ResourceLoader::getShader("occlusion");

    std::string path = "/home/conrad/c++Projects/Final Day On Earth/resources/images/ground";
    forestGroundTEX = new Texture("forest.png", path, false);
    grassTEX = new Texture("grassRock.png", path, false);
    rockTEX = new Texture("rocks.jpg", path, false);
    forestGroundTEXNormal = new Texture("forestNormal.png", path, false);
    grassTEXNormal = new Texture("grassRockNormal.png", path, false);
    rockTEXNormal = new Texture("rocksNormal.png", path, false);

    setUp();
}

void printMatrix(const glm::mat4 &matrix) {
    const float *pMatrix = glm::value_ptr(matrix);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << pMatrix[j * 4 + i] << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << "----------------------------------------------" << std::endl;
}

void TerrainRenderer::renderLocation(Location *location, Camera &camera, Camera &perspective , double time, std::vector<TerrainNode *> nodes) {

    this->location = location;

    Terrain &quadTree = location->quadTree;

    if (quadTree.isActive()) {
        // Terrain
        terrainShader->use();
        glm::mat4 view = perspective.getCameraMatrix();
        glm::mat4 proj = perspective.getProjectionMatrix();

        terrainShader->setMat4("viewMatrix", view);
        terrainShader->setMat4("projectionMatrix", proj);
        terrainShader->setInt("heightMap", 0);
        terrainShader->setInt("ground", 1);
        terrainShader->setInt("grass", 2);
        terrainShader->setInt("rock", 3);
        terrainShader->setInt("grassColor", 4);
        terrainShader->setInt("tangentMap", 5);
        terrainShader->setInt("bitangentMap", 6);
        terrainShader->setFloat("heightMapSize", (float) location->heightMapSize);
        terrainShader->setInt("groundNormal", 7);
        terrainShader->setInt("grassNormal", 8);
        terrainShader->setInt("rockNormal", 9);

        location->heightMapTexture->bind(0);
        forestGroundTEX->bind(1);
        grassTEX->bind(2);
        rockTEX->bind(3);

        if (location->hasGrass) {
            location->grassTypeTexture->bind(4);
        }

        location->heightMapTangentTexture->bind(5);
        location->heightMapBitangentTexture->bind(6);

        forestGroundTEXNormal->bind(7);
        grassTEXNormal->bind(8);
        rockTEXNormal->bind(9);

        // Celestial Bodies
        double a = (2 * M_PI / 1440);

        glm::vec3 sunPos = glm::vec3(std::cos(a * (time - 360.0)) * 1000, std::sin(a * (time - 360.0)) * 1000, 0.0f);
        glm::vec3 moonPos = glm::vec3(-std::cos(a * (time - 360.0)), -std::sin(a * (time - 360.0)), 0.0f);

        // Eventually use a realistic orbit
        terrainShader->setVec3("sunPos", sunPos);
        terrainShader->setVec3("moonPos", moonPos);


        // Get nodes to be rendered

        int nodeCount = nodes.size();

        // Get node data
        std::vector<glm::mat4> matrices(nodeCount);
        std::vector<int> edges(nodeCount);

        for (int i = 0; i < nodeCount; ++i) {
            matrices[i] = nodes[i]->modelMatrix;
            edges[i] = nodes[i]->edge;
        }

        // Set Buffers
        glBindBuffer(GL_ARRAY_BUFFER, patchInstanceMatricesBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * matrices.size(), matrices.data());

        glBindBuffer(GL_ARRAY_BUFFER, patchInstanceEdgesBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(int) * edges.size(), edges.data());


        // Set Flags
        glPatchParameteri(GL_PATCH_VERTICES, 4);

        // Bind VAO
        glBindVertexArray(patchVAO);

        // Render
        glDrawArraysInstanced(GL_PATCHES, 0, 4, nodeCount);

        // Unbind
        glBindVertexArray(0);


    }

    /*basicShader.use();
    basicShader.setMat4("view", viewCamera.getCameraMatrix());
    basicShader.setMat4("projection", projectionMatrix);
    basicShader.setMat4("model", glm::mat4(1));

    camera.drawFrustum(shapeRenderer, basicShader);*/
}


void TerrainRenderer::renderGrass(Location *location, Camera &camera,Camera &perspective, double time, std::vector<TerrainNode *> nodes) {
    // Grass
    if (location->hasGrass) {
        // Celestial Bodies
        double a = (2 * M_PI / 1440);

        glm::vec3 sunPos = glm::vec3(std::cos(a * (time - 360.0)) * 1000, std::sin(a * (time - 360.0)) * 1000, 0.0f);
        glm::vec3 moonPos = glm::vec3(-std::cos(a * (time - 360.0)), -std::sin(a * (time - 360.0)), 0.0f);

        std::vector<glm::vec4> tileList{getGrassTileList(nodes, camera)};
        if (!tileList.empty()) {
            // Set Flags
            occlusionCull(camera, tileList);
            renderGrass(camera, perspective, time, sunPos);
        }
    }
}

void TerrainRenderer::addToList(TerrainNode *node, std::vector<glm::vec4> &tileList, Camera &camera) {
    // Distance to the center of the node
    float dist = glm::distance(glm::vec3(node->centerX, (node->high + node->low) / 2, node->centerZ), camera.getLocation());

    // length of the diagonal
    float extent = std::sqrt((node->size * 0.70710678118) * (node->size * 0.70710678118) + (node->high - node->low) * (node->high - node->low) / 4);

    if (dist - extent > maxGrassDistance)
        return;

    // Assumes nodes can have a size of grassTileSize - If it is less, grass tiles will overlap
    if (node->size > grassTileSize && node->child1 != nullptr) {
        addToList(node->child1, tileList, camera);
        addToList(node->child2, tileList, camera);
        addToList(node->child3, tileList, camera);
        addToList(node->child4, tileList, camera);
        return;
    }

    tileList.emplace_back(node->x, node->z, node->low, node->high - node->low);
}

std::vector<glm::vec4> TerrainRenderer::getGrassTileList(std::vector<TerrainNode *> nodes, Camera &camera) {
    std::vector<glm::vec4> tileList;
    for (TerrainNode *node: nodes) {
        if (node->size > 4 * grassTileSize)
            continue;

        addToList(node, tileList, camera);
    }
    return tileList;
}

void TerrainRenderer::renderGrass(Camera &camera, Camera& perspective, double time, glm::vec3 sunPos) {
    // Reset Counter
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, renderGrassIndirectCommandBuffer);
    DrawArraysIndirectCommand commands[] = {{9, 0, 0, 0}, // High LOD
                                            {5, 0, 0, 0}}; // Low LOD
    glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(commands), &commands[0]);

    // Bind Data
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, grassInstanceBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, grassInstanceBufferLOD);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, grassInputBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, renderGrassIndirectCommandBuffer);

    // Gen blades
    genBlades->use();

    genBlades->setFloat("scale", grassTileSize / (32.0f * grassDensity));

    Frustum frustum = camera.buildFrustum();

    for (int i = 0; i < 6; i++) {
        genBlades->setVec3("planes[" + std::to_string(i) + "].normal", (*(((Plane *) &frustum) + i)).normal);
        genBlades->setFloat("planes[" + std::to_string(i) + "].distance", (*(((Plane *) &frustum) + i)).distance);
    }

    genBlades->setInt("groundHeight", 0);
    genBlades->setFloat("heightMapSize", (float) location->heightMapSize);

    genBlades->setVec3("cameraPos", camera.getLocation());

    location->heightMapTexture->bind(0);

    glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, computeGrassIndirectCommandBuffer);
    glDispatchComputeIndirect((GLuint) 0);

    glMemoryBarrier(GL_ALL_BARRIER_BITS);


    // Render
   // for(int i = 0; i < 2; i++) {
        grassShader->use();

        glm::mat4 view = perspective.getCameraMatrix();
        glm::mat4 proj = perspective.getProjectionMatrix();


        grassShader->setMat4("viewMatrix", view);
        grassShader->setMat4("projectionMatrix", proj);
        grassShader->setInt("groundHeight", 0);
        grassShader->setInt("groundNormal", 2);
        grassShader->setInt("wind", 3);
        grassShader->setFloat("heightMapSize", (float) location->heightMapSize);

        grassShader->setFloat("time", (float) time * 60); // Wind is in in-game time
        grassShader->setVec3("viewVector", camera.forward);

        grassShader->setVec3("sunPos", sunPos);
        grassShader->setVec3("cameraPos", camera.getLocation());

        location->heightMapTexture->bind(0);
        location->heightMapNormalTexture->bind(2);
        location->wind->bind(3);


        // Render High Lod Grass
        glBindVertexArray(grassVAO);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, renderGrassIndirectCommandBuffer);

        glDrawArraysIndirect(GL_TRIANGLE_STRIP, (void *) 0);

        // Render Low LOD Grass
        glBindVertexArray(grassVAOLOD);

        glDrawArraysIndirect(GL_TRIANGLE_STRIP, (void *) sizeof(DrawArraysIndirectCommand));

        // Unbind VAO
        glBindVertexArray(0);
       // grassShader.swapID(grassNormalShader);
   // }

}

void TerrainRenderer::occlusionCull(Camera &camera, std::vector<glm::vec4> &tileList) {
    glm::mat4 view = camera.getCameraMatrix();
    glm::mat4 proj = camera.getProjectionMatrix();

    // Reset Visibility
    // Perhaps could store an array of zeros maxGrassTiles long and just copy from that each time
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, visibilityBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * tileList.size(), zeros.data());

    // Send Tile Positions
    glBindBuffer(GL_ARRAY_BUFFER, tileInputBuffer);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * tileList.size(), tileList.data());

    // Unbind Buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Bind Visibility Buffer
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, visibilityBuffer);

    // Disable Writing to Color and Depth Buffers - Might be redundant do to not setting FragColor in shader
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);

    // Render Bounding Boxes
    occludeBoundingBoxShader->use();


    occludeBoundingBoxShader->setMat4("view", view);
    occludeBoundingBoxShader->setMat4("projection", proj);
    occludeBoundingBoxShader->setFloat("tileSize", grassTileSize);

    glBindVertexArray(boxVAO);

    glDrawElementsInstanced(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_INT, 0, tileList.size());

    // Unbind VAO
    glBindVertexArray(0);

    // Enable Writing to Color and Depth Buffers
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);

    // MAKES SURE VISIBILITY BUFFER IS UPDATED BEFORE NEXT STAGE
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Set Dispatch Command
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, computeGrassIndirectCommandBuffer);
    ComputeShaderIndirectCommand command{static_cast<uint>(grassDensity), static_cast<uint>(grassDensity), 0};
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(ComputeShaderIndirectCommand), &command);

    // Bind Data
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, visibilityBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, tileInputBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, grassInputBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, computeGrassIndirectCommandBuffer);

    // Run Compute Shader to generate indirect call to genBlades
    occlusionCulling->use();

    glDispatchCompute(1, 1, tileList.size());

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

TerrainRenderer::~TerrainRenderer() {
    glDeleteVertexArrays(1, &patchVAO);
    glDeleteBuffers(1, &patchVBO);
    glDeleteBuffers(1, &patchInstanceEdgesBuffer);
    glDeleteBuffers(1, &patchInstanceMatricesBuffer);

    glDeleteVertexArrays(1, &grassVAO);
    glDeleteBuffers(1, &bladeVBO);
    glDeleteBuffers(1, &grassInstanceBuffer);
    glDeleteBuffers(1, &renderGrassIndirectCommandBuffer);
    glDeleteBuffers(1, &grassInputBuffer);
    glDeleteBuffers(1, &tileInputBuffer);

    glDeleteVertexArrays(1, &grassVAOLOD);
    glDeleteBuffers(1, &bladeVBOLOD);
    glDeleteBuffers(1, &grassInstanceBufferLOD);

    glDeleteBuffers(1, &boxVAO);
    glDeleteBuffers(1, &boxVBO);
    glDeleteBuffers(1, &boxEBO);

    delete forestGroundTEX;
    delete grassTEX;
    delete rockTEX;
    delete forestGroundTEXNormal;
    delete grassTEXNormal;
    delete rockTEXNormal;

    // Shaders self delete when they go out of scope
}

// It is very important to unbind the VAO after creating it or later commands can screw it up in unpredictable ways
void TerrainRenderer::setUp() {

    // Build Patch VAO
    glGenVertexArrays(1, &patchVAO);
    glBindVertexArray(patchVAO);

    glGenBuffers(1, &patchVBO);
    glBindBuffer(GL_ARRAY_BUFFER, patchVBO);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float) * squareVertices.size(), &squareVertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);

    // Edges Buffer
    glGenBuffers(1, &patchInstanceEdgesBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, patchInstanceEdgesBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(int) * maxTerrainTiles, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_INT, GL_FALSE, sizeof(int), (void *) 0);

    // Matrices Buffer
    glGenBuffers(1, &patchInstanceMatricesBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, patchInstanceMatricesBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * maxTerrainTiles, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *) 0);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *) (sizeof(glm::vec4)));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *) (2 * sizeof(glm::vec4)));

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *) (3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);

    // Unbind VAO
    glBindVertexArray(0);

    // Create Grass Compute Buffers
    glGenBuffers(1, &grassInputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, grassInputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, maxGrassTiles * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &renderGrassIndirectCommandBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, renderGrassIndirectCommandBuffer);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, 2 * sizeof(DrawArraysIndirectCommand), nullptr, GL_DYNAMIC_DRAW);


    // TODO :: set grass buffer to shader storage and see performance impact

    // Occlusion Buffers
    glGenBuffers(1, &visibilityBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, visibilityBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, maxGrassTiles * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &computeGrassIndirectCommandBuffer);
    glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, computeGrassIndirectCommandBuffer);
    glBufferData(GL_DISPATCH_INDIRECT_BUFFER, sizeof(ComputeShaderIndirectCommand), nullptr, GL_DYNAMIC_DRAW);

    // Unbind Buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Build Box VAO
    float boxVertices[] = {
            1.0, 1.0, 1.0,
            0.0f, 1.0, 1.0,
            1.0, 1.0, 0.0f,
            0.0f, 1.0, 0.0f,
            1.0, 0.0f, 1.0,
            0.0f, 0.0f, 1.0,
            0.0f, 0.0f, 0.0f,
            1.0, 0.0f, 0.0f
    };

    unsigned int boxIndices[] = {
            3, 2, 6, 7, 4, 2, 0,
            3, 1, 6, 5, 4, 1, 0
    };

    // Create Box VAO
    glGenVertexArrays(1, &boxVAO);
    glBindVertexArray(boxVAO);

    glGenBuffers(1, &boxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), &boxVertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &boxEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxIndices), &boxIndices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

    glGenBuffers(1, &tileInputBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tileInputBuffer);
    glBufferData(GL_ARRAY_BUFFER, maxGrassTiles * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glVertexAttribDivisor(1, 1);

    // Unbind Everything
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    buildGrassVAO();
    buildLowLODGrassVAO();
}

void TerrainRenderer::buildGrassVAO() {
    // Build Grass Blade VBO
    float grassBladeVertices[] = {
            1.0f, 0.0f,
            0.0f, 0.0f,
            0.95f, 0.3f,
            0.05f, 0.3f,
            0.9f, 0.65f,
            0.1f, 0.65f,
            0.85f, 0.9f,
            0.15f, 0.9f,
            0.5f, 1.0f
    };

    // Create Grass Blade VAO
    glGenVertexArrays(1, &grassVAO);
    glBindVertexArray(grassVAO);

    // Build VBO
    glGenBuffers(1, &bladeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, bladeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grassBladeVertices), &grassBladeVertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);

    // Create Grass Instance Buffer
    glGenBuffers(1, &grassInstanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, grassInstanceBuffer);
    glBufferData(GL_ARRAY_BUFFER, maxBladesPerTile * maxGrassTiles * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);


    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));

    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);

    // Unbind VAO
    glBindVertexArray(0);
}

void TerrainRenderer::buildLowLODGrassVAO() {
    // Build Grass Blade VBO
    float grassBladeVertices[] = {
            1.0f, 0.0f,
            0.0f, 0.0f,
            1.0f, 0.75f,
            0.0f, 0.75f,
            0.5f, 1.0f
    };

    // Create Grass Blade VAO
    glGenVertexArrays(1, &grassVAOLOD);
    glBindVertexArray(grassVAOLOD);

    // Build VBO
    glGenBuffers(1, &bladeVBOLOD);
    glBindBuffer(GL_ARRAY_BUFFER, bladeVBOLOD);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grassBladeVertices), &grassBladeVertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);

    // Create Grass Instance Buffer
    glGenBuffers(1, &grassInstanceBufferLOD);
    glBindBuffer(GL_ARRAY_BUFFER, grassInstanceBufferLOD);
    glBufferData(GL_ARRAY_BUFFER, maxBladesPerTile * maxGrassTiles * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);


    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));

    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);

    // Unbind VAO
    glBindVertexArray(0);
}

