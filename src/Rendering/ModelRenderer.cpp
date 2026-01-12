#include "ModelRenderer.h"

#include <utility>
#include <iostream>
#include "../Utility/ResourceLoader.h"
#include "glad/glad.h"

ModelRenderer::ModelRenderer(std::string name, unsigned int instanceData) : modelName(name), instanceData(instanceData) {
}

ModelRenderer::~ModelRenderer() {
    unload();
}

void ModelRenderer::render(int startIndex, int number) {
    if(!loaded) {
        std::cout << "Model is not currently loaded\n";
        return;
    }

    for(int i = 0; i < model->meshes.size(); i++){
        ModelMesh &mesh = model->meshes[i];
        if(mesh.diffuse != -1)
        textures[mesh.diffuse]->bind(0);
        if(mesh.specular != -1)
        textures[mesh.specular]->bind(1);
        if(mesh.normal != -1)
        textures[mesh.normal]->bind(2);

        glBindVertexArray(VAOS[i]);
        glDrawElementsInstancedBaseInstance(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0, number, startIndex);
        glBindVertexArray(0);
    }

}

void ModelRenderer::load() {
    if(loaded)
        unload(); // Will reload
    loaded = true;

    if(model == nullptr)
        model = ResourceLoader::getModel(modelName);

    for(int i = 0; i < model->textureCount; i++)
        textures.push_back(new Texture(std::to_string(i) + ".png", model->texturePath, false));


    for(ModelMesh mesh : model->meshes){
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh.vertices.size(), &mesh.vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.indices.size(), &mesh.indices[0], GL_STATIC_DRAW);

        // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);

        // Texture Coordinates
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (sizeof(float) * 3));

        // Model Matrix
        glBindBuffer(GL_ARRAY_BUFFER, instanceData);

        std::size_t vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        glVertexAttribDivisor(2, 1);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
        glVertexAttribDivisor(3, 1);

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glVertexAttribDivisor(4, 1);

        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
        glVertexAttribDivisor(5, 1);

        VAOS.push_back(VAO);
        VBOS.push_back(VBO);
        EBOS.push_back(EBO);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void ModelRenderer::unload() {
    if(!loaded)
        return;
    loaded = false;

    glDeleteVertexArrays(VAOS.size(), VAOS.data());
}