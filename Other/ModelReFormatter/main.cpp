#include <iostream>
#include <fstream>
#include <vector>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>
#include <filesystem>
#include <set>

using namespace std;

struct Vertex{
    float x;
    float y;
    float z;
    float textureX;
    float textureY;
};

struct Layer{
    std::vector<unsigned int> indices;
    std::vector<Vertex> vertices;
    unsigned int diffuse = -1;
    unsigned int normal = -1;
    unsigned int specular = -1;
};

std::map<int, std::string> mergeTextures;

std::vector<Layer> layers;
std::map<std::string, int> textures;

void processMaterials(const aiScene * aiScene);
void processMeshes(const aiScene *scene);
int addTexture(std::string path);

void reverseWinding(Layer *layer);
bool isWindingCCW(Layer *layer);

void mergeTexture(std::string path1, std::string path2);

bool flipMesh = false;
bool noMat1 = false;

int main() {

    cout << "Enter model path : ";
    string p;
    cin >> p;
    cout << "Use explicit texture folder (y/n) : ";
    string e;
    cin >> e;
    cout << "Reverse winding order? (y/n) : ";
    string g;
    cin >> g;
    flipMesh = g == "y";
    cout << "Skip first material? (y/n) : ";
    cin >> g;
    noMat1 = g == "y";

    string folderPath;
    bool tF = false;
    if(e == "y"){
        cout << "Folder Path : ";
        cin >> folderPath;
        tF = true;
    }

    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(p, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << "\n";
        return 0;
    }
    std::string directory = p.substr(0, p.find_last_of('/'));

    processMaterials(scene);
    processMeshes(scene);

    map<string, int>::iterator it;

    for (it = textures.begin(); it != textures.end(); it++){
        cout << it->first << " ";
    }
    cout << "\n";

    std::ofstream outfile(p.substr(p.find_last_of('/') + 1, p.find_last_of('.') - p.find_last_of('/') - 1) + ".AMG", std::ofstream::binary);

    unsigned int textureCount = textures.size();
    outfile.write(reinterpret_cast<char *>(&textureCount), sizeof(textureCount));

    unsigned int layerCount = layers.size();
    outfile.write(reinterpret_cast<char *>(&layerCount), sizeof(layerCount));

    for(Layer &layer : layers){
        // Write diffuse, normal, and specular textures
        outfile.write(reinterpret_cast<char *>(&layer.diffuse), sizeof(layer.diffuse));
        outfile.write(reinterpret_cast<char *>(&layer.normal), sizeof(layer.normal));
        outfile.write(reinterpret_cast<char *>(&layer.specular), sizeof(layer.specular));

        // Write vertex count and vertices
        unsigned int vertexCount = layer.vertices.size();
        outfile.write(reinterpret_cast<char *>(&vertexCount), sizeof(vertexCount));
        outfile.write(reinterpret_cast<char *>(layer.vertices.data()), sizeof(Vertex) * vertexCount);

        // Write index count and indices
        unsigned int indexCount = layer.indices.size();
        outfile.write(reinterpret_cast<char *>(&indexCount), sizeof(indexCount));
        outfile.write(reinterpret_cast<char *>(layer.indices.data()), sizeof(unsigned int) * indexCount);
    }

    outfile.close();

    // Copy textures

    std::string textureFolder = "textures";
    std::filesystem::create_directory(textureFolder);

    for (const auto& [n, value] : textures) {
        std::string newTextureName = textureFolder + "/" + std::to_string(value) + ".png";

        std::string name = n;

        std::replace(name.begin(), name.end(), '\\', '/');

        std::string path = directory + "/" + name;

        auto trim = [](std::string& str) {
            const auto start = str.find_first_not_of(" \t\n\r\f\v");
            const auto end = str.find_last_not_of(" \t\n\r\f\v");
            str = (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
        };

        trim(path);

        if(tF)
            path = folderPath + "/" + (name.find_last_of('/') == std::string::npos ? name : name.substr(name.find_last_of('/') + 1));

        if(mergeTextures.count(value) > 0){
            name = mergeTextures[value];

            std::replace(name.begin(), name.end(), '\\', '/');

            std::string path2 = directory + "/" + name;
            if(tF)
                path2 = folderPath + "/" + (name.find_last_of('/') == std::string::npos ? name : name.substr(name.find_last_of('/') + 1));

            mergeTexture(path, path2);
        }

        std::ifstream src(path, std::ios::binary);
        std::ofstream dst(newTextureName, std::ios::binary);

        if (!src) {
            std::cerr << "Error: Could not open source file " << path << "\n";
            continue;
        }

        if (!dst) {
            std::cerr << "Error: Could not create destination file " << newTextureName << "\n";
            continue;
        }

        dst << src.rdbuf();
        src.close();
        dst.close();
    }

    return 0;
}

void processMeshes(const aiScene *scene)
{

    std::set<int> usedLayers;

    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];

        // For some reason a blank material appears at index zero - It is removed
        int index = mesh->mMaterialIndex - noMat1;

        if(index < 0){
            cout << "ERROR : mesh " << i << " does not have a material" << endl;
            return;
        }

        usedLayers.insert(index);

        int Vsize = layers[index].vertices.size();

        for (unsigned int y = 0; y < mesh->mNumVertices; y++) {
            Vertex vertex{};

            vertex.x = mesh->mVertices[y].x;
            vertex.y = mesh->mVertices[y].y;
            vertex.z = mesh->mVertices[y].z;

            if (mesh->mTextureCoords[0]) {
                vertex.textureX = mesh->mTextureCoords[0][y].x;
                vertex.textureY = mesh->mTextureCoords[0][y].y;
            }

            layers[index].vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];

            for (unsigned int j = 0; j < face.mNumIndices; j++)
                layers[index].indices.push_back(Vsize + face.mIndices[j]);
        }
    }

    std::vector<Layer> filteredLayers;

    for (int i = 0; i < layers.size(); i++) {
        if (usedLayers.count(i)) {
            filteredLayers.push_back(layers[i]);
        }
    }
//Enter model path : /home/conrad/Downloads/Skull_v3_L2.123c1407fc1e-ea5c-4cb9-9072-d28b8aba4c36/12140_Skull_v3_L2.obj
    layers = std::move(filteredLayers);

    for(Layer &layer : layers){
        if(flipMesh || !isWindingCCW(&layer))
            reverseWinding(&layer);
    }

}

class Vec3 {
public:
    float x, y, z;

    Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f)
            : x(x), y(y), z(z) {}

    // Addition
    Vec3 operator+(const Vec3 &other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    // Subtraction
    Vec3 operator-(const Vec3 &other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    // Cross product
    Vec3 cross(const Vec3 &other) const {
        return Vec3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
        );
    }

    // Div scalar
    Vec3 operator/(const float &i) const {
        return {x / i, y / i, z / i};
    }

    Vec3 operator*(const float &i) const {
        return {x * i, y * i, z * i};
    }

    float dot(const Vec3 &other) const {
        return x * other.x + y * other.y + z * other.z;
    }

};

double SignedVolume(Vec3 a,Vec3 b, Vec3 c, Vec3 d) {
    return (1.0/6.0)*(b-a).cross(c-a).dot(d-a);
}

int sign(float a){
    if(a > 0)
        return 1;
    if(a < 0)
        return -1;
    return 0;
}

bool isWindingCCW(Layer *layer){
    Vec3 vec0(layer->vertices[layer->indices[0]].x, layer->vertices[layer->indices[0]].y, layer->vertices[layer->indices[0]].z);
    Vec3 vec1(layer->vertices[layer->indices[1]].x, layer->vertices[layer->indices[1]].y, layer->vertices[layer->indices[1]].z);
    Vec3 vec2(layer->vertices[layer->indices[2]].x, layer->vertices[layer->indices[2]].y, layer->vertices[layer->indices[2]].z);

    Vec3 side1 = vec1 - vec0;
    Vec3 side2 = vec2 - vec1;

    Vec3 normal = side1.cross(side2) * 100000.0f;
    Vec3 center = (vec0 + vec1 + vec2) / 3.0f;

    int count = 0;

    // Test normal vec for intersection on every triangle then count all in front of the center (based on normal) even = good, odd = flipped
    for(int i = 3 ; i < layer->indices.size(); i += 3){
        Vec3 a = Vec3(layer->vertices[layer->indices[i]].x, layer->vertices[layer->indices[i]].y, layer->vertices[layer->indices[i]].z);
        Vec3 b = Vec3(layer->vertices[layer->indices[i + 1]].x, layer->vertices[layer->indices[i + 1]].y, layer->vertices[layer->indices[i + 1]].z);
        Vec3 c = Vec3(layer->vertices[layer->indices[i + 2]].x, layer->vertices[layer->indices[i + 2]].y, layer->vertices[layer->indices[i + 2]].z);

        if(SignedVolume(normal, a, b, c) * SignedVolume(center, a, b, c) < 0){
            if(sign(SignedVolume(normal, center, a, b)) == sign(SignedVolume(normal, center, b, c)) == sign(SignedVolume(normal, center, c, a))){
                count++;
            }
        }

    }

    return count % 2 == 0;
}

void reverseWinding(Layer *layer) {
    std::cout << "MESH was flipped!!\n";
    for(int i = 0; i < layer->indices.size() / 3; i++){
        unsigned int temp = layer->indices[i * 3 + 1];
        layer->indices[i * 3 + 1] = layer->indices[i * 3 + 2];
        layer->indices[i * 3 + 2] = temp;
    }
}

void processMaterials(const aiScene *scene)
{
    for(int i = noMat1; i < scene->mNumMaterials; i++){
        Layer layer;

        aiMaterial *material = scene->mMaterials[i];

        aiString str;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
        if(str.length != 0)
            layer.diffuse = addTexture(str.C_Str());

        material->GetTexture(aiTextureType_HEIGHT, 0, &str);
        if(str.length != 0)
            layer.normal = addTexture(str.C_Str());

        material->GetTexture(aiTextureType_SPECULAR, 0, &str);
        if(str.length != 0)
            layer.specular = addTexture(str.C_Str());

        material->GetTexture(aiTextureType_BASE_COLOR, 0, &str);
        if(str.length != 0)
            layer.diffuse = addTexture(str.C_Str());

        material->GetTexture(aiTextureType_OPACITY, 0, &str);
        if(str.length != 0 && layer.diffuse != -1)
            mergeTextures[layer.diffuse] = str.C_Str();

        if(layer.diffuse == -1)
            std::cout << "Diffuse texture does not exist for layer " << i << "\n";

        layers.push_back(layer);
    }
}

int addTexture(std::string path){
    if(textures.count(path))
        return textures[path];
    return textures[path] = textures.size();
}

void mergeTexture(std::string path1, std::string path2){
    std::cout << path1 << " and " << path2 << " need to be merged\n";
}