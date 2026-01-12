#include <iostream>
#include <fstream>
#include <filesystem>
#include "Model.h"

Model::Model(std::string path) {
    std::ifstream infile(path, std::ifstream::binary);
    if (!infile) {
        std::cerr << "Error opening file: " << path << std::endl;
        return;
    }

    // Construct Meshes
    infile.read(reinterpret_cast<char *>(&textureCount), sizeof(textureCount));

    unsigned int layerCount;
    infile.read(reinterpret_cast<char *>(&layerCount), sizeof(layerCount));

    for (int i = 0; i < layerCount; i++) {
        unsigned int diffuse;
        infile.read(reinterpret_cast<char *>(&diffuse), sizeof(diffuse));
        unsigned int normal;
        infile.read(reinterpret_cast<char *>(&normal), sizeof(normal));
        unsigned int specular;
        infile.read(reinterpret_cast<char *>(&specular), sizeof(specular));

        // Read vertex count and vertices
        unsigned int vertexCount;
        infile.read(reinterpret_cast<char *>(&vertexCount), sizeof(vertexCount));
        std::vector<Vertex> vertices(vertexCount);
        infile.read(reinterpret_cast<char *>(vertices.data()), sizeof(Vertex) * vertexCount);

        // Read index count and indices
        unsigned int indexCount;
        infile.read(reinterpret_cast<char *>(&indexCount), sizeof(indexCount));
        std::vector<unsigned int> indices(indexCount);
        infile.read(reinterpret_cast<char *>(indices.data()), sizeof(unsigned int) * indexCount);

        meshes.emplace_back(diffuse, normal, specular, vertices, indices);
    }

    infile.close();


    texturePath = path.substr(0, path.find_last_of('/')) + "/textures";
}

Model::~Model() {

}


ModelMesh::ModelMesh(unsigned int i, unsigned int i1, unsigned int i2, std::vector<Vertex> vector1, std::vector<unsigned int> vector2) : diffuse(i), normal(i), specular(i), vertices(vector1), indices(vector2){

}
