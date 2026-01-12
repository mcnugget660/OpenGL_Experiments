#ifndef FINALDAYONEARTH_MODEL_H
#define FINALDAYONEARTH_MODEL_H

#include <vector>
#include <string>

// All CPU

struct Vertex{
    float x;
    float y;
    float z;
    float textureX;
    float textureY;
};

struct ModelMesh{
    ModelMesh(unsigned int i, unsigned int i1, unsigned int i2, std::vector<Vertex> vector1, std::vector<unsigned int> vector2);

    unsigned int diffuse = -1;
    unsigned int normal = -1;
    unsigned int specular = -1;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

class Model{
public:
    Model() = default;

    ~Model();

    explicit Model(std::string path);

    unsigned int textureCount = 0;
    std::string texturePath;
    std::vector<ModelMesh> meshes;

};

#endif //FINALDAYONEARTH_MODEL_H
