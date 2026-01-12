#ifndef FINALDAYONEARTH_TERRAINRENDERER_H
#define FINALDAYONEARTH_TERRAINRENDERER_H

#include "Shader.h"
#include "Texture.h"
#include "TriangleMeshTEX.h"
#include "../Scene/Terrain.h"
#include "../Game/Location/Location.h"
#include "glm/ext/matrix_transform.hpp"


class TerrainRenderer {
public:
    TerrainRenderer();

    ~TerrainRenderer();

    void renderLocation(Location *location, Camera &camera, Camera &perspective, double time, std::vector<TerrainNode *> nodes);

    void renderGrass(Location *location, Camera &camera, Camera &perspective, double time, std::vector<TerrainNode *> nodes);

private:
    Shader* terrainShader;
    Shader* grassShader;
    Shader* grassNormalShader;

    Shader* genBlades;
    Shader* occlusionCulling;

    Shader* basicShader;
    Shader* occludeBoundingBoxShader;

    Texture *rockTEX;
    Texture *rockTEXNormal;
    Texture *forestGroundTEX;
    Texture *forestGroundTEXNormal;
    Texture *grassTEX;
    Texture *grassTEXNormal;

    // Terrain
    unsigned int patchVAO = -1;
    unsigned int patchVBO = -1;
    unsigned int patchInstanceEdgesBuffer = -1;
    unsigned int patchInstanceMatricesBuffer = -1;

    static std::vector<glm::vec2> squareVertices;

    int maxTerrainTiles = 150;

    // Grass
    unsigned int grassVAO = -1;
    unsigned int bladeVBO = -1;
    unsigned int grassInstanceBuffer = -1;

    unsigned int grassVAOLOD = -1;
    unsigned int bladeVBOLOD = -1;
    unsigned int grassInstanceBufferLOD = -1;

    unsigned int renderGrassIndirectCommandBuffer = -1;

    unsigned int grassInputBuffer = -1;
    unsigned int computeGrassIndirectCommandBuffer = -1;

    int grassDensity = 16; // How many 32x32 grass chunks make up an edge of a Tile
    int grassTileSize = 16;

    // The compute shader operates on 32 x 32 grass blades per tile
    int maxBladesPerTile = 32 * 32 * grassDensity * grassDensity;

    int maxGrassDistance = 80;
    // Could replace 4 with pi
    int maxGrassTiles = 4 * (maxGrassDistance / grassTileSize + 1) * (maxGrassDistance / grassTileSize + 1);

    // Occlusion
    unsigned int visibilityBuffer = -1;
    unsigned int tileInputBuffer = -1;

    unsigned int boxVAO = -1;
    unsigned int boxVBO = -1;
    unsigned int boxEBO = -1;

    std::vector<unsigned int> zeros;

    Location *location = nullptr;

    void setUp();

    void buildGrassVAO();

    void buildLowLODGrassVAO();

    void occlusionCull(Camera &camera, std::vector<glm::vec4> &tileList);

    void renderGrass(Camera &camera, Camera &perspective, double time, glm::vec3 sunPos);

    std::vector<glm::vec4> getGrassTileList(std::vector<TerrainNode *> nodes, Camera &camera);

    void addToList(TerrainNode *node, std::vector<glm::vec4> &tileList, Camera &camera);
};


#endif //FINALDAYONEARTH_TERRAINRENDERER_H
