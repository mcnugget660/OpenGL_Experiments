#include "Terrain.h"

#include <map>
#include <iostream>
#include "../Rendering/Texture.h"
#include "../Utility/PerlinNoise.h"
#include "glm/ext/matrix_transform.hpp"
#include "Camera.h"

bool Terrain::isActive() const {
    return active;
}

void Terrain::setActive(bool value) {
    Terrain::active = value;
}

// Ensure size is a power of 2
Terrain::Terrain(unsigned int size) : QuadTree(std::pow(2, std::floor(std::log2(size)))), rootNode(0, 0, getSize(), nullptr) {

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
int TerrainNode::frustumCull(Frustum *frustum, int in_mask, int *out_mask) {
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

TerrainNode::TerrainNode(int x, int z, unsigned int size, TerrainNode *parent) : Node(x, z, size), parent(parent) {
    modelMatrix = glm::translate(glm::mat4(1), glm::vec3(x, 0, z)) * glm::scale(glm::mat4(1), glm::vec3(size, 1.0, size));
}


void TerrainNode::initalizeFromHeightMap(float *heightMap, unsigned int heightMapSize) {
    if (size == smallestNode) {
        // Each point on the grid has a heightmap value
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                double height = heightMap[(x + i) * heightMapSize + (z + j)];
                low = std::min(low, height);
                high = std::max(high, height);
            }
        }

        setBounds();
        return;
    }

    int half = size / 2;
    child1 = new TerrainNode(x, z, half, this);
    child2 = new TerrainNode(x, z + half, half, this);
    child3 = new TerrainNode(x + half, z, half, this);
    child4 = new TerrainNode(x + half, z + half, half, this);

    child1->initalizeFromHeightMap(heightMap, heightMapSize);
    child2->initalizeFromHeightMap(heightMap, heightMapSize);
    child3->initalizeFromHeightMap(heightMap, heightMapSize);
    child4->initalizeFromHeightMap(heightMap, heightMapSize);
}


void TerrainNode::split(Camera &camera, Frustum *frustum, int in_mask, std::vector<TerrainNode *> &nodes) {
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
    bool split = (dist - 0.70710678118 * size < 20 * size / smallestNode);

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


void TerrainNode::setBounds() {
    if (parent == nullptr)
        return;

    ((TerrainNode *) parent)->low = std::min(((TerrainNode *) parent)->low, low);
    ((TerrainNode *) parent)->high = std::max(((TerrainNode *) parent)->high, high);

    ((TerrainNode *) parent)->setBounds();
}

void Terrain::getNodes(Camera &camera, std::vector<TerrainNode *> &nodes) {
    Frustum frustum = camera.buildFrustum();
    rootNode.split(camera, &frustum, 63, nodes);
}

void Terrain::initalizeFromHeightMap(float *heightMap, unsigned int heightMapSize) {
    rootNode.initalizeFromHeightMap(heightMap, heightMapSize);
}

TerrainNode::~TerrainNode() {
    delete child1;
    delete child2;
    delete child3;
    delete child4;
}
