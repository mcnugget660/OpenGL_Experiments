#ifndef FINALDAYONEARTH_TERRAIN_H
#define FINALDAYONEARTH_TERRAIN_H

#include "QuadTree.h"

#include <cfloat>
#include <vector>

#include "glm/glm.hpp"

class Camera;

class Frustum;

inline unsigned int smallestNode = 16;

struct TerrainNode : public Node {
    TerrainNode(int x, int z, unsigned int size, TerrainNode *parent);

    ~TerrainNode();

    int frustumCull(Frustum *frustum, int in_mask, int *out_mask);

    void initalizeFromHeightMap(float *heightMap, unsigned int heightMapSize);

    void split(Camera &camera, Frustum *frustum, int in_mask, std::vector<TerrainNode *> &nodes);

    void setBounds();

    TerrainNode *child1 = nullptr;
    TerrainNode *child2 = nullptr;
    TerrainNode *child3 = nullptr;
    TerrainNode *child4 = nullptr;

    TerrainNode *parent = nullptr;

    double low = DBL_MAX;
    double high = -DBL_MAX;

    int edge = 0;

    int failPlane = 0;

    glm::mat4 modelMatrix;
};

class Terrain : public QuadTree {
public:

    Terrain(unsigned int size);

    void initalizeFromHeightMap(float *heightMap, unsigned int heightMapSize);

    void getNodes(Camera &camera, std::vector<TerrainNode *> &nodes);

    bool isActive() const;

    void setActive(bool value);

private:
    bool active = false;

    TerrainNode rootNode;
};


#endif //FINALDAYONEARTH_TERRAIN_H
