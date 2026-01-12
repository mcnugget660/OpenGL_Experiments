
#include <iostream>
#include <vector>
#include <fstream>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

struct Vertex{
    float x;
    float y;
    float z;
    float textureX;
    float textureY;
};

struct ModelMesh{
    unsigned int diffuse;
    unsigned int normal;
    unsigned int specular;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

void writeFile(string path, vector<ModelMesh>& meshes, unsigned int textureCount, unsigned int layerCount);

int main() {
    cout << "Enter Path : ";
    string path;
    cin >> path;

    vector<ModelMesh> meshes;

    std::ifstream infile(path, std::ifstream::binary);
    if (!infile) {
        std::cerr << "Error opening file: " << path << std::endl;
        return 0;
    }

    unsigned int textureCount;
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

        meshes.push_back({diffuse, normal, specular, vertices, indices});
    }

    infile.close();

    // Make alterations
    double mx = 0, sx = 1, rx = 0;
    double my = 0, sy = 1, ry = 0;
    double mz = 0, sz = 1, rz = 0;
    cout << "Translate Model? (y/n) :";
    string response;
    cin >> response;
    if(response == "y"){
        cout << "X translation : ";
        cin >> response;
        mx = std::stod(response);
        cout << "Y translation : ";
        cin >> response;
        my = std::stod(response);
        cout << "Z translation : ";
        cin >> response;
        mz = std::stod(response);
    }

    cout << "Scale Model? (y/n) :";
    cin >> response;
    if(response == "y"){
        cout << "X scale : ";
        cin >> response;
        sx = std::stod(response);
        cout << "Y scale : ";
        cin >> response;
        sy = std::stod(response);
        cout << "Z scale : ";
        cin >> response;
        sz = std::stod(response);
    }

    cout << "Rotate Model? (y/n) :";
    cin >> response;
    if (response == "y") {
        cout << "X rotation (degrees) : ";
        cin >> response;
        rx = std::stod(response);
        cout << "Y rotation (degrees) : ";
        cin >> response;
        ry = std::stod(response);
        cout << "Z rotation (degrees) : ";
        cin >> response;
        rz = std::stod(response);
    }

    glm::mat4 matrix(1.0f);

    // Apply rotation first
    matrix = glm::rotate(matrix, (float) glm::radians(rz), glm::vec3(0.0f, 0.0f, 1.0f));  // Z rotation
    matrix = glm::rotate(matrix, (float) glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));  // Y rotation
    matrix = glm::rotate(matrix, (float) glm::radians(rx), glm::vec3(1.0f, 0.0f, 0.0f));  // X rotation

    // Apply scale and translation
    matrix = glm::scale(matrix, glm::vec3(sx, sy, sz));
    matrix = glm::translate(matrix, glm::vec3(mx, my, mz));

    for (ModelMesh &mesh : meshes) {
        for (Vertex &v : mesh.vertices) {
            glm::vec3 v2 = glm::vec3(matrix * glm::vec4(v.x, v.y, v.z, 1.0f));
            v.x = v2.x;
            v.y = v2.y;
            v.z = v2.z;
        }
    }


    writeFile(path.substr(0, path.length() - 4) + "NEW.AMG", meshes, textureCount, layerCount);

    return 0;
}

void writeFile(string path, vector<ModelMesh>& meshes, unsigned int textureCount, unsigned int layerCount){
    std::ofstream outfile(path, std::ofstream::binary);

    outfile.write(reinterpret_cast<char *>(&textureCount), sizeof(textureCount));
    outfile.write(reinterpret_cast<char *>(&layerCount), sizeof(layerCount));

    for(ModelMesh &mesh : meshes){
        // Write diffuse, normal, and specular textures
        outfile.write(reinterpret_cast<char *>(&mesh.diffuse), sizeof(mesh.diffuse));
        outfile.write(reinterpret_cast<char *>(&mesh.normal), sizeof(mesh.normal));
        outfile.write(reinterpret_cast<char *>(&mesh.specular), sizeof(mesh.specular));

        // Write vertex count and vertices
        unsigned int vertexCount = mesh.vertices.size();
        outfile.write(reinterpret_cast<char *>(&vertexCount), sizeof(vertexCount));
        outfile.write(reinterpret_cast<char *>(mesh.vertices.data()), sizeof(Vertex) * vertexCount);

        // Write index count and indices
        unsigned int indexCount = mesh.indices.size();
        outfile.write(reinterpret_cast<char *>(&indexCount), sizeof(indexCount));
        outfile.write(reinterpret_cast<char *>(mesh.indices.data()), sizeof(unsigned int) * indexCount);
    }

    outfile.close();
}