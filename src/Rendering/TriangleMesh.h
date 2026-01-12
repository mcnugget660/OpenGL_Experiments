#ifndef FINALDAYONEARTH_TRIANGLEMESH_H
#define FINALDAYONEARTH_TRIANGLEMESH_H

#include <vector>
#include "Vertices.h"

class TriangleMesh {
public:
    TriangleMesh(std::vector<Vertex3f> vertices, std::vector<unsigned int> indices);

    TriangleMesh();

    ~TriangleMesh();

    void draw() const;

protected:
    unsigned int VAO = -1;
    unsigned int VBO = -1;
    unsigned int EBO = -1;

    int numIndices = 0;
};

class TriangleMeshColor : public TriangleMesh {
public:
    TriangleMeshColor(std::vector<Vertex6f> vertices, std::vector<unsigned int> indices);

};

#endif //FINALDAYONEARTH_TRIANGLEMESH_H
