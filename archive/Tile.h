#ifndef A_TILE_H
#define A_TILE_H

#include <vector>
#include <cfloat>
#include <map>
#include "glm/glm.hpp"
#include "../Rendering/Texture.h"
#include "../Utility/PerlinNoise.h"

class Camera;

struct Frustum;

inline unsigned int smallestNode = 16;

struct Node {
    Node(int x, int z, unsigned int size, Node *parent);

    ~Node();

    void initalize(float *heightMap, unsigned int heightMapSize);

    void setBounds();

    void split(Camera &camera, Frustum *frustum, int in_mask, std::vector<Node *> &nodes);

    int frustumCull(Frustum *frustum, int in_mask, int *out_mask);

    unsigned int size{};
    int x{};
    int z{};
    int centerX{};
    int centerZ{};

    double low = DBL_MAX;
    double high = -DBL_MAX;

    int edge = 0;

    int failPlane = 0;

    glm::mat4 modelMatrix;

    Node *child1 = nullptr;
    Node *child2 = nullptr;
    Node *child3 = nullptr;
    Node *child4 = nullptr;

    Node *parent = nullptr;
};

class GrassChunk;


class Tile {
    friend class LocationRenderer;

public:
    Tile(float *heightMap, int heightMapSize, int tileSize, Texture *grassTypes, Texture *wind);

    Tile(float *heightMap, unsigned int size);

    ~Tile();

    void getNodes(Camera &camera, std::vector<Node *> &nodes);

    double getHeightAt(double x, double z);

    bool isActive() const;

    void setActive(bool value);

    int getSize();

private:
    float *heightMap; // X{Z, Z, Z, Z, Z, Z}
    Texture *heightMapTexture;
    Texture *heightMapNormalTexture;
    int heightMapSize = -1;

    int size;
    Node rootNode;

    bool hasGrass = false;
    Texture *grassTypeTexture;
    Texture *wind;

    bool active = false;

    bool isUnderground = false;

    void setUpTile();
};


#endif //A_TILE_H
