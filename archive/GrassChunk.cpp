#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

#include <vector>
#include <iostream>
#include <list>
#include <cfloat>
#include <glm/vec3.hpp>
#include <valarray>

#include "GrassChunk.h"
#include "glad/glad.h"

#include "../Utility/PerlinNoise.h"
#include "../Rendering/RenderUtil.h"
#include "../Utility/ResourceLoader.h"
#include "../Utility/MathUtil.h"
#include "../Rendering/Shader.h"

unsigned int GrassChunk::bladeVBO = -1;
unsigned int GrassChunk::bladeLowLODVBO = -1;

GrassChunk::GrassChunk(int span, float sampleRate, bool lowLOD) : size(span) {
    if (sampleRate <= 0.0)
        sampleRate = 1.0;

    if (lowLOD)
        sampleRate *= 2; // Twice as wide, twice as sparse

    std::vector<glm::vec3> points;

    float variance = 0.05;

    for (int i = 0; i < span / sampleRate; i++) {
        for (int j = 0; j < span / sampleRate; j++) {
            glm::vec2 loc = glm::vec2(i * sampleRate + ((float) rand() / RAND_MAX - 0.5) * variance,
                                      j * sampleRate + ((float) rand() / RAND_MAX - 0.5) * variance);

            points.emplace_back(loc.x, loc.y, rand()); // Position
        }
    }

    blades = points.size();

    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * blades, &points[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    createVAO(lowLOD);
}

GrassChunk::~GrassChunk() {
    glDeleteVertexArrays(1, &GrassVAO);
    glDeleteBuffers(1, &instanceVBO);
}

void GrassChunk::render() {
    glBindVertexArray(GrassVAO);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 15, blades);
    glBindVertexArray(0);
}

void GrassChunk::createVAO(bool lowLOD) {
    glGenVertexArrays(1, &GrassVAO);
    glBindVertexArray(GrassVAO);

    glBindBuffer(GL_ARRAY_BUFFER, lowLOD ? bladeLowLODVBO : bladeVBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glVertexAttribDivisor(1, 1); // tell OpenGL this is an instanced vertex attribute.

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) (2 * sizeof(float)));
    glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void GrassChunk::setUp() {
    std::string registerName = "grassBladeVBO";
    if (!ResourceLoader::hasVBO(registerName)) {
        float grassBladeVertices[] = {
                1.0f, 0.0f,
                0.0f, 0.0f,
                1.0f, 0.15f,
                0.0f, 0.15f,
                0.95f, 0.3f,
                0.05f, 0.3f,
                0.95f, 0.475f,
                0.05f, 0.475f,
                0.9f, 0.65f,
                0.1f, 0.65f,
                0.9f, 0.75f,
                0.1f, 0.75f,
                0.85f, 0.9f,
                0.15f, 0.9f,
                0.5f, 1.0f
        };

        glGenBuffers(1, &bladeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, bladeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(grassBladeVertices), &grassBladeVertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        ResourceLoader::registerVBO(registerName, bladeVBO);
    }

    registerName = "grassBladeLowLODVBO";
    if (!ResourceLoader::hasVBO(registerName)) {
        float grassBladeVertices[] = {
                4.0f, 0.0f,
                0.0f, 0.0f,
                2.0f, 1.0f
        };

        glGenBuffers(1, &bladeLowLODVBO);
        glBindBuffer(GL_ARRAY_BUFFER, bladeLowLODVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(grassBladeVertices), &grassBladeVertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        ResourceLoader::registerVBO(registerName, bladeLowLODVBO);
    }
}

int GrassChunk::getBlades() const {
    return blades;
}

int GrassChunk::getSize() const {
    return size;
}

void GrassChunk::renderMultible(std::vector<glm::vec2> &chunks) {
    glBindVertexArray(GrassVAO);

    // Send data
    int chunkCount = chunks.size();
    unsigned int VBO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * chunkCount, chunks.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *) 0);
    glVertexAttribDivisor(3, blades);

    // Draw Call
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 15, blades * chunkCount);

    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
}
