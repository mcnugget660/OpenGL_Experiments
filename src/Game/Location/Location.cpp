#include <iostream>
#include "Location.h"
#include "../../Scene/QuadTree.h"
#include "../../Utility/TextureUtil.h"

Location::Location(Biome biome, Size size) : biome(biome), size(size), quadTree(size) {

}

std::vector<glm::vec3> Location::setTerrain(float *h, int hs) {
    heightMap = h;
    heightMapSize = hs;

    quadTree.setActive(true);
    return initalizeHeightMap();
}

void Location::setGrass(Texture *t, Texture *w) {
    grassTypeTexture = t;
    wind = w;

    hasGrass = true;
}

int Location::getSize() {
    return size;
}

std::vector<glm::vec3> Location::initalizeHeightMap() {
    quadTree.initalizeFromHeightMap(heightMap, heightMapSize);
    heightMapTexture = TextureUtil::floatArray2DToTexture(heightMap, heightMapSize, false);

    std::vector<glm::vec3> normals(heightMapSize * heightMapSize, glm::vec3(0.0f, 0.0f, 0.0f));

    for (int x = 0; x < heightMapSize - 1; x++) {
        for (int z = 0; z < heightMapSize - 1; z++) {
            glm::vec3 faceNormal = glm::normalize(glm::cross(glm::vec3(0, heightMap[heightMapSize * x + z + 1] - heightMap[heightMapSize * x + z], 1),
                                              glm::vec3(1, heightMap[heightMapSize * (x + 1) + z] - heightMap[heightMapSize * x + z], 0)));

            normals[heightMapSize * x + z] += faceNormal;
            normals[heightMapSize * x + z + 1] += faceNormal;
            normals[heightMapSize * (x + 1) + z] += faceNormal;
            normals[heightMapSize * (x + 1) + z + 1] += faceNormal;
        }
    }

    for(glm::vec3 &normal : normals)
        normal = glm::normalize(normal);

    std::vector<unsigned char> convertedNormals(3 * heightMapSize * heightMapSize);
    std::vector<unsigned char> convertedTangents(3 * heightMapSize * heightMapSize);
    std::vector<unsigned char> convertedBitangents(3 * heightMapSize * heightMapSize);

    for (int x = 0; x < heightMapSize; x++) {
        for (int z = 0; z < heightMapSize; z++) {
            glm::vec3 normal = normals[x * heightMapSize + z];
            glm::vec3 tangent;
            if(normal.y < 0.999)
                tangent = glm::normalize(glm::cross(normal, {0, 1, 0}));
            else
                tangent = {1,0,0}; // Not sure what to put here


            glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));

            convertedNormals[heightMapSize * x * 3 + z * 3] = (normal.x * 0.5f + 0.5f) * 255;
            convertedNormals[heightMapSize * x * 3 + z * 3 + 1] = (normal.y * 0.5f + 0.5f) * 255;
            convertedNormals[heightMapSize * x * 3 + z * 3 + 2] = (normal.z * 0.5f + 0.5f) * 255;

            convertedTangents[heightMapSize * x * 3 + z * 3] = (tangent.x * 0.5f + 0.5f) * 255;
            convertedTangents[heightMapSize * x * 3 + z * 3 + 1] = (tangent.y * 0.5f + 0.5f) * 255;
            convertedTangents[heightMapSize * x * 3 + z * 3 + 2] = (tangent.z * 0.5f + 0.5f) * 255;

            convertedBitangents[heightMapSize * x * 3 + z * 3] = (bitangent.x * 0.5f + 0.5f) * 255;
            convertedBitangents[heightMapSize * x * 3 + z * 3 + 1] = (bitangent.y * 0.5f + 0.5f) * 255;
            convertedBitangents[heightMapSize * x * 3 + z * 3 + 2] = (bitangent.z * 0.5f + 0.5f) * 255;
        }
    }

    heightMapNormalTexture = TextureUtil::colorArray2DToTexture(convertedNormals, heightMapSize);
    heightMapTangentTexture = TextureUtil::colorArray2DToTexture(convertedTangents, heightMapSize);
    heightMapBitangentTexture = TextureUtil::colorArray2DToTexture(convertedBitangents, heightMapSize);

    return normals;
}

double area(double x1, double y1, double x2, double y2, double x3, double y3) {
    return 0.5 * std::abs(x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
}

float Location::heightMapAt(float x, float z) {
    x = std::min(std::max(0.0f, x), size - 1.0f);
    z = std::min(std::max(0.0f, z), size - 1.0f);

    float xPart = x - (int) x;
    float zPart = z - (int) z;

    double vertex1Height = heightMap[(int) x * heightMapSize + std::min(heightMapSize - 1, (int) z + 1)];
    double vertex2Height = heightMap[std::min(heightMapSize - 1, (int) x + 1) * heightMapSize + (int) z];

    bool bottomTriangle = xPart + zPart > 1;
    double vertex3Height = heightMap[std::min(heightMapSize - 1, ((int) x + bottomTriangle)) * heightMapSize + std::min(heightMapSize - 1, (int) z + bottomTriangle)];

    double totalArea = 0.5;

    double areaPBC = area(xPart, zPart, 0, 1, bottomTriangle, bottomTriangle);
    double areaPCA = area(xPart, zPart, bottomTriangle, bottomTriangle, 1, 0);

    // Barycentric coordinates (lambda1, lambda2, lambda3)
    double lambda1 = areaPBC / totalArea; // Z+ Corner
    double lambda2 = areaPCA / totalArea; // X+ Corner
    double lambda3 = 1 - lambda1 - lambda2;

    return vertex1Height * lambda2 + vertex2Height * lambda1 + vertex3Height * lambda3;
}


void Location::setTime(double hours, double minutes) {
    time = fmod(60 * hours + minutes, 1440.0);
}

void Location::advanceTime(double hours, double minutes) {
    time = fmod(time + 60 * hours + minutes, 1440.0);
}

void Location::addResource(ModelID modelId, ResourceData resourceData, std::vector<glm::vec2> positions) {
    resourceLocations[modelId] = positions;
    resourceInfo[modelId] = resourceData;
}
/*
EntityID Location::createEntity(glm::vec2 pos, glm::vec2 direction, std::string type) {
    return 0;
}*/
