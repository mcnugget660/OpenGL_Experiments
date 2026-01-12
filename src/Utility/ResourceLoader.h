#ifndef FINALDAYONEARTH_RESOURCELOADER_H
#define FINALDAYONEARTH_RESOURCELOADER_H

#include <string>
#include <map>
#include "../Rendering/Model.h"
#include "../Rendering/Shader.h"
#include "../GUI/FontManager.h"
#include "JsonReader.h"

enum RESOURCE_TYPE{
    SHADER,
    MODEL,
    JSON,
    TEXTURE,
    VAO,
    VBO,
    FONT
};

class ResourceLoader {
public:
    static JSON_OBJECT* getJSON(std::string path);

    static Model* getModel(std::string name);

    static Shader* getShader(std::string name);

    static bool hasVAO(std::string id);

    static void registerVAO(std::string id, unsigned int VAO);

    static unsigned int getVAO(std::string id);

    static bool hasVBO(std::string id);

    static void registerVBO(std::string id, unsigned int VBO);

    static unsigned int getVBO(std::string id);

    static void freeVAO(std::string id);

    static void freeVBO(std::string id);

    static void freeJsons();

    static void freeAllResources();

    static FontManager& getFontManager() {return *fontManager;}

    static void loadConfigs();

    static void refreshResources();

private:
    static std::string resourcePath;

    static std::map<std::string, unsigned int> vaoMap;
    static std::map<std::string, unsigned int> vboMap;

    static std::map<std::string, Model> models;

    static std::map<std::string, Shader*> shaders;

    static std::map<std::string, RESOURCE_TYPE> resourceDirectory;

    static FontManager* fontManager;

    // Config
    static std::map<std::string, std::pair<SHADER_TYPE, std::vector<std::string>>> shaderConfigs;

    static std::map<std::string, JSON_OBJECT> jsons;
};


#endif //FINALDAYONEARTH_RESOURCELOADER_H
