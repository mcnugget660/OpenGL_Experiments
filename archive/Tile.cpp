#include "glm/ext/matrix_transform.hpp"
#include "Tile.h"
#include "../Rendering/GrassChunk.h"
#include "Camera.h"
#include "../Utility/TextureUtil.h"

#include <valarray>
#include <iostream>

Node::Node(int x, int z, unsigned int size, Node *parent) : x(x), z(z), size(size), centerX(x + size / 2), centerZ(z + size / 2), parent(parent) {
    modelMatrix = glm::translate(glm::mat4(1), glm::vec3(x, 0, z)) * glm::scale(glm::mat4(1), glm::vec3(size, 1.0, size));
}

Node::~Node() {
    delete child1;
    delete child2;
    delete child3;
    delete child4;
}

// 0 is outside | 1 is intersecting | 2 is inside
int intersectsPlane(glm::vec3 center, glm::vec3 extent, glm::vec3 planeNormal, float distance) {
    // The magnitude of the vertices to the center is the same and the angles with the normal of the plane that would make that vector furthest away are the same
    // so the dot product of the equivalent normal in the first quadrant will output the same absolute component values as that same plane rotated
    // around the cube with the new opposite vertex
    // The furthest vertex is at most 90 degrees away from the plane and (to end up with a positive value) is in front of the plane - this allows us
    // to pretend both vectors lie in the first quadrant (the axes would then be treated as absolute values so the resulting dot-product will be the full distance
    // from the projected vertex on the normal to the origin)
    double range = extent.x * std::abs(planeNormal.x) + extent.y * std::abs(planeNormal.y) + extent.z * std::abs(planeNormal.z);

    double centerOffset = glm::dot(center, planeNormal) - distance;

    // If the center at most range less than the plane along its normal the box intersects the plane
    if (centerOffset + range < 0) {
        return 0; // Completely outside
    }
    if (centerOffset - range > 0) {
        return 2; // Completely inside
    }
    return 1; // Intersecting
}

//in_mask is a list of planes that were intersected by the parent node
int Node::frustumCull(Frustum *frustum, int in_mask, int *out_mask) {
    *out_mask = 0;

    // Frustum Cull
    glm::vec3 center(centerX, (high + low) / 2, centerZ);
    glm::vec3 extent(size / 2, (high - low) / 2, size / 2);

    int i;
    int k = 1 << failPlane; //2^failPlane
    int result = 2;

    // Test previous failed plane first
    if (k & in_mask) {
        Plane *plane = ((Plane *) frustum) + failPlane;
        result = intersectsPlane(center, extent, plane->normal, plane->distance);
        if (result == 0)
            return 0;
        if (result == 1)
            *out_mask |= k;
    }

    // loop through all unchecked planes
    for (i = 0, k = 1; k <= in_mask; i++, k += k) {
        if (i != failPlane && k & in_mask) {
            Plane *plane = ((Plane *) frustum) + i;
            result = intersectsPlane(center, extent, plane->normal, plane->distance);
            if (result == 0) {
                failPlane = i;
                return 0;
            }
            if (result == 1)
                *out_mask |= k; // Smaller nodes might be outside the boundary in this case
        }
    }
    return result;
}

void Node::split(Camera &camera, Frustum *frustum, int in_mask, std::vector<Node *> &nodes) {
    int out_mask = 0;

    // If node is not fully inside frustum
    if (in_mask != 0) {
        int result = frustumCull(frustum, in_mask, &out_mask);
        if (result == 0)
            return;
    }

    if (size == smallestNode) {
        nodes.push_back(this);
        return;
    }

    double dist = std::abs(
            glm::distance(glm::vec2(camera.getLocation().x, camera.getLocation().z), glm::vec2(centerX, centerZ)));
    //Distance to circle with radius sqrt(2) * size / 2
    bool split = (dist - 0.70710678118 * size < 10 * size / smallestNode);

    if (split) {
        // Decide what edges need to be up-scaled
        // These are the correct edges assuming forwards is positive z and right is positive x
        // These don't match the expected tess edges because of the patch vertex order -- It just works
        /*
        int RIGHT = 1;
        int TOP = 2;
        int LEFT = 4;
        int BOTTOM = 8;
        */

        child1->split(camera, frustum, out_mask, nodes);
        child2->split(camera, frustum, out_mask, nodes);
        child3->split(camera, frustum, out_mask, nodes);
        child4->split(camera, frustum, out_mask, nodes);

        return;
    }

    // TODO : Set edges correctly to remove cracks in the terrain

    // Doesn't split
    // Just make all edges on non-smallest quads 2x to avoid cracks
    // Could remove edges on adjacent and sibling nodes if they both don't split - probably less expensive to just render some extra triangles
    edge = 15;

    nodes.push_back(this);
}

void Node::initalize(float *heightMap, unsigned int heightMapSize) {
    if (size == smallestNode) {
        // Each point on the grid has a heightmap value
        for (int i = 0; i <= size; i++) {
            for (int j = 0; j <= size; j++) {
                double height = heightMap[(x + i) * heightMapSize + (z + j)];
                low = std::min(low, height);
                high = std::max(high, height);
            }
        }

        setBounds();
        return;
    }

    int half = size / 2;
    child1 = new Node(x, z, half, this);
    child2 = new Node(x, z + half, half, this);
    child3 = new Node(x + half, z, half, this);
    child4 = new Node(x + half, z + half, half, this);

    child1->initalize(heightMap, heightMapSize);
    child2->initalize(heightMap, heightMapSize);
    child3->initalize(heightMap, heightMapSize);
    child4->initalize(heightMap, heightMapSize);
}

void Node::setBounds() {
    if (parent == nullptr)
        return;

    parent->low = std::min(parent->low, low);
    parent->high = std::max(parent->high, high);

    parent->setBounds();
}

Tile::Tile(float *heightMap, int heightMapSize, int tileSize, Texture *grassTypes, Texture *wind) :
        heightMap(heightMap), size(std::pow(2, std::floor(std::log2(tileSize)))),
        heightMapSize(heightMapSize), rootNode(0, 0, Tile::size, nullptr), hasGrass(true), grassTypeTexture(grassTypes), wind(wind) {
    setUpTile();
}

Tile::Tile(float *heightMap, unsigned int size) :
        heightMap(heightMap), size(std::pow(2, std::floor(std::log2(size)))),
        heightMapSize(size + 1), rootNode(0, 0, Tile::size, nullptr), hasGrass(false) {
    setUpTile();
}

Tile::~Tile() {
    std::cout << "Tile Deleted\n";
    delete (heightMapTexture);
    delete[] heightMap;
}

// Split triangle into three triangles and multiply each vertex by the area of the triangle formed by the other two vertices over the total area of the triangle
double Tile::getHeightAt(double x, double z) {
    int nx = std::min(heightMapSize - 2.0, std::max(0.0, x));
    int nz = std::min(heightMapSize - 2.0, std::max(0.0, z));

    double corner2 = heightMap[nx * heightMapSize + nz + 1];
    double corner3 = heightMap[(nx + 1) * heightMapSize + nz];

    if ((z - ((int) z)) + (x - ((int) x)) > 1) { // Top Triangle
        double corner1 = heightMap[nx * heightMapSize + nz];

        double a2 = 1 * (x - ((int) x)) * 0.5;
        double a3 = 1 * (z - ((int) z)) * 0.5;
        double a1 = 0.5 - a2 - a3;

        //The area is a total of 0.5 so the sub-area ratio is twice the sub-area
        return corner1 * 2 * a1 + corner2 * 2 * a2 + corner3 * 2 * a3;
    }
    // Bottom Triangle
    double corner4 = heightMap[(nx + 1) * heightMapSize + nz + 1];

    // Triangle heights are flipped
    double a2 = 1 * (1 - (x - ((int) x))) * 0.5;
    double a3 = 1 * (1 - (z - ((int) z))) * 0.5;
    double a4 = 0.5 - a2 - a3;

    return corner2 * 2 * a2 + corner3 * 2 * a3 + corner4 * 2 * a4;
}

bool Tile::isActive() const {
    return active;
}

void Tile::setActive(bool value) {
    Tile::active = value;
}

void Tile::setUpTile() {
    rootNode.initalize(heightMap, heightMapSize);
    heightMapTexture = TextureUtil::floatArray2DToTexture(heightMap, heightMapSize, false);

    std::vector<glm::vec3> normals(heightMapSize * heightMapSize, glm::vec3(0.0f, 0.0f, 0.0f));
    for (int i = 0; i < heightMapSize - 1; i++) {
        for (int j = 0; j < heightMapSize - 1; j++) {
            glm::vec3 faceNormal = glm::cross(glm::vec3(0, heightMap[heightMapSize * i + j + 1] - heightMap[heightMapSize * i + j], 1),
                                              glm::vec3(1, heightMap[heightMapSize * (i + 1) + j] - heightMap[heightMapSize * i + j], 0));

            normals[heightMapSize * i + j] += faceNormal;
            normals[heightMapSize * i + j + 1] += faceNormal;
            normals[heightMapSize * (i + 1) + j] += faceNormal;
            normals[heightMapSize * (i + 1) + j + 1] += faceNormal;
        }
    }

    std::vector<unsigned char> convertedNormals(3 * heightMapSize * heightMapSize);
    for (int i = 0; i < heightMapSize; i++) {
        for (int j = 0; j < heightMapSize; j++) {
            glm::vec3 normal = glm::normalize(normals[i * heightMapSize + j]);
            convertedNormals[heightMapSize * i * 3 + j * 3] = (normal.x * 0.5f + 0.5f) * 255;
            convertedNormals[heightMapSize * i * 3 + j * 3 + 1] = (normal.y * 0.5f + 0.5f) * 255;
            convertedNormals[heightMapSize * i * 3 + j * 3 + 2] = (normal.z * 0.5f + 0.5f) * 255;
        }
    }

    heightMapNormalTexture = TextureUtil::colorArray2DToTexture(convertedNormals, heightMapSize);
}

int Tile::getSize() {
    return size;
}

void Tile::getNodes(Camera &camera, std::vector<Node *> &nodes) {
    Frustum frustum = camera.buildFrustum();
    rootNode.split(camera, &frustum, 63, nodes);
}
