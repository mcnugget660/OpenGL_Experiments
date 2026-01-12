#ifndef FINALDAYONEARTH_LOCATION_H
#define FINALDAYONEARTH_LOCATION_H

#include <unordered_map>
#include "glm/fwd.hpp"
#include "../../Scene/QuadTree.h"
#include "../../Scene/Terrain.h"
#include "../../Rendering/Texture.h"
#include "../Entity/EntityComponentStorage.h"

enum Biome {
    Grassland,
    Savanna,
    Empty
};

enum Size {
    SMALL = 256,
    MEDIUM = 512,
    LARGE = 1024
};

enum Difficulty {
    Easy,
    Medium,
    Hard,
    Boss,
    Unknown
};

struct ResourceData{
    double radius = 0;
};

typedef unsigned int ModelID;

struct Location {
    double temperature = 0.0;
    double humidity = 0.0;

    double time = 0;

    Biome biome = Empty;
    Size size = SMALL;

    // Terrain
    Terrain quadTree;

    float *heightMap; // X{Z, Z, Z, Z, Z, Z}
    Texture *heightMapTexture;
    Texture *heightMapNormalTexture;
    Texture *heightMapTangentTexture;
    Texture *heightMapBitangentTexture;
    int heightMapSize = -1;

    bool hasGrass = false;
    Texture *grassTypeTexture;
    Texture *wind;

    // Entity Data
    unsigned int entityCount = 0;

    EntityComponentStorage ECS;

    // Data Structures
    std::vector<std::vector<std::pair<std::vector<glm::vec2>,std::vector<unsigned int >>>> localObjects;
    std::vector<std::vector<std::pair<std::vector<glm::vec2>,std::vector<unsigned int >>>> resourseOnlyGridStructure;

    // Resources
    std::vector<std::string> staticModels;
    std::vector<std::string> dynamicModels;

    std::unordered_map<ModelID, std::vector<EntityID>> entityModelInstances;
    std::unordered_map<ModelID, std::vector<glm::vec2>> resourceLocations;
    std::unordered_map<ModelID, ResourceData> resourceInfo;

    std::unordered_map<unsigned int, std::vector<EntityID>> sdfInstances;


    explicit Location(Biome biome, Size size);

    std::vector<glm::vec3> setTerrain(float *heightMap, int heightMapSize);

    void setGrass(Texture *grassTypeTexture, Texture *wind);

    int getSize();

    std::vector<glm::vec3> initalizeHeightMap();

    float heightMapAt(float x, float z);

    void setTime(double hours, double minutes);

    void advanceTime(double hours, double minutes);

    void addResource(ModelID modelId, ResourceData resourceData, std::vector<glm::vec2> positions);
};

#endif //FINALDAYONEARTH_LOCATION_H