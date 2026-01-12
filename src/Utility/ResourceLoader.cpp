#include <iostream>
#include <filesystem>
#include "ResourceLoader.h"
#include "glad/glad.h"
#include "JsonReader.h"

std::map<std::string, unsigned int> ResourceLoader::vaoMap;
std::map<std::string, unsigned int> ResourceLoader::vboMap;
std::map<std::string, Model> ResourceLoader::models;
std::map<std::string, Shader*> ResourceLoader::shaders;
std::map<std::string, std::pair<SHADER_TYPE, std::vector<std::string>>> ResourceLoader::shaderConfigs;
std::map<std::string, JSON_OBJECT> ResourceLoader::jsons;
FontManager* ResourceLoader::fontManager = nullptr;

std::string ResourceLoader::resourcePath = "/home/conrad/c++Projects/Final Day On Earth/resources/";

bool ResourceLoader::hasVAO(std::string id) {
    return vaoMap.count(id);
}

void ResourceLoader::registerVAO(std::string id, unsigned int VAO) {
    if (vaoMap.count(id))
        std::cout << "ResourceLoader :: VAO '" + id + "' was replaced\n";
    vaoMap[id] = VAO;
}

bool ResourceLoader::hasVBO(std::string id) {
    return vboMap.count(id);
}

void ResourceLoader::registerVBO(std::string id, unsigned int VBO) {
    if (vboMap.count(id))
        std::cout << "ResourceLoader :: VBO '" + id + "' was replaced\n";
    vaoMap[id] = VBO;
}

unsigned int ResourceLoader::getVAO(std::string id) {
    return vaoMap[id];
}

unsigned int ResourceLoader::getVBO(std::string id) {
    return vboMap[id];
}

void ResourceLoader::freeAllResources() {
    for (auto &item: vaoMap) {
        glDeleteVertexArrays(1, &item.second);
    }
    for (auto &item: vboMap) {
        glDeleteBuffers(1, &item.second);
    }
    vaoMap.clear();
    vboMap.clear();

    for(auto& [key, value] : shaders)
        delete(value);
    shaderConfigs.clear();

    fontManager->freeFonts();
    delete fontManager;
}

void ResourceLoader::freeVAO(std::string id) {
    if (vaoMap.count(id))
        glDeleteVertexArrays(1, &vaoMap[id]);
    else
        std::cout << "ResourceLoader :: tried to free non-existant VAO '" + id + "'\n";
}

void ResourceLoader::freeVBO(std::string id) {
    if (vboMap.count(id))
        glDeleteBuffers(1, &vboMap[id]);
    else
        std::cout << "ResourceLoader :: tried to free non-existant VBO '" + id + "'\n";
}

Model *ResourceLoader::getModel(std::string name) {
    if (models.count(name))
        return &models.at(name);

    return &(models[name] = Model("/home/conrad/c++Projects/Final Day On Earth/resources/models/" + name));
}


Shader *ResourceLoader::getShader(std::string name) {
    if (shaders.count(name))
        return shaders.at(name);

    if(!shaderConfigs.count(name)) {
        std::cout << "ResourceLoader :: shader '" + name + "' does not exist\n";
        return nullptr;
    }
    Shader* p = Shader::buildShader(shaderConfigs.at(name).first, shaderConfigs.at(name).second);
    return (shaders[name] = p);
}

void ResourceLoader::loadConfigs() {
    JSON_OBJECT shader_config = JsonReader::read(resourcePath + "shaders/shaders.json"); // Do not store in jsons vector for reloading purposes
    std::map<string, vector<std::string>>& map = shader_config.string_fields;
    for(auto& [key, value] : map) {
        SHADER_TYPE type = key == "compute" ? COMPUTE : key == "basic" ? BASIC : key == "geometry" ? GEOMETRY : SHADER_TYPE::ERROR;
        if(type == SHADER_TYPE::ERROR) {
            std::cout << "ResourceLoader :: shader_config.json contains invalid shader type '" + key + "'\n";
            continue;
        }
        for(std::string sh : value) {
            int valid = 1;
            std::string p = resourcePath + "shaders/" + key + "/" + sh;

            std::vector<std::string> paths;

            if (type == COMPUTE) {
                if (valid *= std::filesystem::exists(p + ".comp"))
                    paths.push_back(p + ".comp");
            } else {
                if(valid *= std::filesystem::exists(p + ".vert"))
                    paths.push_back(p + ".vert");;
                if(valid *= std::filesystem::exists(p + ".frag"))
                    paths.push_back(p + ".frag");;
            }
            if(type == GEOMETRY) {
                if (valid *= std::filesystem::exists(p + ".tesc"))
                    paths.push_back(p + ".tesc");
                if (std::filesystem::exists(p + ".tese"))
                    paths.push_back(p + ".tese"); // Not required
            }

            if(valid){
                shaderConfigs.insert({sh, {type, paths}});
            } else {
                std::cout << "ResourceLoader :: shader '" + sh + "' does not have required files for type '" + key + "'\n";
            }
        }
    }

    ResourceLoader::fontManager = new FontManager(resourcePath + "fonts/");
}

JSON_OBJECT *ResourceLoader::getJSON(std::string path) {
    if(jsons.count(path))
        return &jsons.at(path);
    return &(jsons[path] = JsonReader::read(resourcePath + path));
}

void ResourceLoader::freeJsons() {
    jsons.clear();
}

// Will only refresh files loaded from the resource folder (usage of these files is assumed to remain unchanged)
void ResourceLoader::refreshResources() {
    // Refresh existing shaders (referenced in presumably unchanged code)
    /*
    for(auto& [key, value] : shaders) {
        glDeleteShader(value->getId());
        Shader *p = Shader::buildShader(shaderConfigs.at(key).first, shaderConfigs.at(key).second);
    }

    for(std::string &name : sh)
        getShader(name);
    */
    // Refresh JSONS
    std::vector<std::string> js;
    for(auto& [key, value] : jsons)
        js.push_back(key);
    freeJsons();
    for(std::string &path : js)
        getJSON(path);
}

