#include "LocationGenerator.h"
#include "../../Utility/PerlinNoise.h"
#include "../../Utility/MathUtil.h"
#include "../../Scene/QuadTree.h"
#include "../../Utility/TextureUtil.h"
#include "../Entity/EntityLoader.h"

#include <random>
#include <iostream>

Location *
LocationGenerator::generateRandomLocation(Difficulty difficulty, Biome biome, Size size, int seed) {
    PerlinNoise noise;
    noise.createGradient(69);

    std::cout << "Seed : " << seed << "\n";

    //Terrain
    double smoothness = 100.0;
    float amplitude = 30.0f;

    // Second Octave
    double smoothness2 = 14.0;
    float amplitude2 = 2.0f;

    float *heightMap = new float[size * size];

    for (int i = 0; i < size; i++) {
        for (int y = 0; y < size; y++) {
            double dx = i / smoothness;
            double dy = y / smoothness;

            double dx2 = i / smoothness2;
            double dy2 = y / smoothness2;

            heightMap[y + i * size] = (float) noise.getPerlin(dx, dy) * amplitude + (float) noise.getPerlin(dx2, dy2) * amplitude2;
        }
    }

    //Grass
    double temperature = 0;
    double humidity = 0;

    PerlinNoise windNoise;
    windNoise.createGradient(PerlinNoise::randomSeed());

    Texture *grassColor = windNoise.asTexture(size, size, 18, 100.1, true);

    Texture *wind = windNoise.asMergedTexture({{2.0f, 20},
                                               {2.0f, 4}}, size, size, true);

    // Construction
    auto *location = new Location(biome, size);
    std::vector<glm::vec3> normals = location->setTerrain(heightMap, size);
    location->setGrass(grassColor, wind);

    // Resource nodes

    // Trees
    std::vector<glm::vec2> treeVec;

    PerlinNoise treeNoise;
    treeNoise.createGradient(PerlinNoise::randomSeed());

    float treeDensity = (rand() % 101) / 1000.0 + 0.5;
    float buffer = 0.1;
    int individualTreeChance = 30; // Amount of failed attempts for each success outside of the buffer

    for(int x = 1; x < size; x += 2) {
        for(int z = 1; z < size; z += 2) {
            float dx = x + rand() % 2;
            float dz = z + rand() % 2;

            float noise = treeNoise.getPerlin(x / 17.0, z / 17.0);

            if(noise < treeDensity || (noise > treeDensity && 0 == rand() % (int) MathUtil::smoothStep(1, individualTreeChance, std::min(1.0f, (noise - treeDensity) / buffer)))) {
                treeVec.emplace_back(dx, dz);
            }
        }
    }

    location->addResource(0, {2.0}, treeVec);

    // Required Models
    location->staticModels.emplace_back("pine-tree/pine-tree.AMG");

    location->dynamicModels.emplace_back("backpack/backpack.AMG");


    for(int i = 0; i < 40; i++) {
        EntityLoader::createEntity({rand() % size, rand() % size}, {rand() % 2 - 1,rand() % 2 - 1}, "Ball", location);
    }

    return location;
}