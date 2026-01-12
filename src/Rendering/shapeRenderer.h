#ifndef SHAPES_H
#define SHAPES_H

#include "glm/vec3.hpp"
#include "Shader.h"

class ShapeRenderer {
public:
    ShapeRenderer() {
        initShapes();
    }

    ~ShapeRenderer() {
        cleanShapes();
    }

    void fillRectangularPrism(glm::vec3 scale, glm::vec3 offset, Shader &shader,
                              glm::vec3 color = glm::vec3(255, 0, 0)) {
        glm::mat4 model_mat = glm::mat4(1.0f);
        model_mat = glm::translate(model_mat, offset);

        model_mat = glm::scale(model_mat, scale);

        shader.setMat4("model", model_mat);

        shader.setVec3("color", color);

        //Actually render
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }


    // Offset is the center and scale is the vector to the positive corner
    void drawRectangularPrism(glm::vec3 scale, glm::vec3 offset, Shader &shader, glm::vec3 color) {
        glm::mat4 model_mat = glm::mat4(1.0f);

        model_mat = glm::translate(glm::mat4(1), offset) * glm::scale(glm::mat4(1), scale);

        shader.setMat4("model", model_mat);

        shader.setVec3("color", color);

        //Actually render
        glBindVertexArray(cubeOutlineVAO);
        glDrawArrays(GL_LINES, 0, 24);
        glBindVertexArray(0);
    }

    void drawTexturedRectangle() {
        glBindVertexArray(squareVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void drawLine(glm::vec3 point1, glm::vec3 point2) {
        float tempVerts[] = {
                point1.x, point1.y, point1.z,
                point2.x, point2.y, point2.z
        };
        unsigned int tempVAO;
        unsigned int tempVBO;
        glGenVertexArrays(1, &tempVAO);
        glGenBuffers(1, &tempVBO);
        glBindVertexArray(tempVAO);
        glBindBuffer(GL_ARRAY_BUFFER, tempVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(tempVerts), &tempVerts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

        glDrawArrays(GL_LINES, 0, 2);

        glDeleteVertexArrays(1, &tempVAO);
        glDeleteBuffers(1, &tempVBO);
        glBindVertexArray(0);
    }

private:
    unsigned int cubeVAO;
    unsigned int cubeVBO;

    unsigned int cubeOutlineVAO;
    unsigned int cubeOutlineVBO;

    unsigned int squareVAO;
    unsigned int squareVBO;

    void cleanShapes() {
        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteBuffers(1, &cubeVBO);
        glDeleteVertexArrays(1, &cubeOutlineVAO);
        glDeleteBuffers(1, &cubeOutlineVBO);
        glDeleteVertexArrays(1, &squareVAO);
        glDeleteBuffers(1, &squareVBO);
    }

    void initShapes() {
        float rectVertices[] = {
                -1.0f,-1.0f,-1.0f, // triangle 1 : begin
                -1.0f,-1.0f, 1.0f,
                -1.0f, 1.0f, 1.0f, // triangle 1 : end
                1.0f, 1.0f,-1.0f, // triangle 2 : begin
                -1.0f,-1.0f,-1.0f,
                -1.0f, 1.0f,-1.0f, // triangle 2 : end
                1.0f,-1.0f, 1.0f,
                -1.0f,-1.0f,-1.0f,
                1.0f,-1.0f,-1.0f,
                1.0f, 1.0f,-1.0f,
                1.0f,-1.0f,-1.0f,
                -1.0f,-1.0f,-1.0f,
                -1.0f,-1.0f,-1.0f,
                -1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f,-1.0f,
                1.0f,-1.0f, 1.0f,
                -1.0f,-1.0f, 1.0f,
                -1.0f,-1.0f,-1.0f,
                -1.0f, 1.0f, 1.0f,
                -1.0f,-1.0f, 1.0f,
                1.0f,-1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f,-1.0f,-1.0f,
                1.0f, 1.0f,-1.0f,
                1.0f,-1.0f,-1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f,-1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f,-1.0f,
                -1.0f, 1.0f,-1.0f,
                1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f,-1.0f,
                -1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f, 1.0f,
                1.0f,-1.0f, 1.0f
        };

        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), &rectVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

        glBindVertexArray(0);

        float rectangleOutlineVertices[] = {
                // Front face
                -1.0f, -1.0f, -1.0f, // Vertex 1
                1.0f, -1.0f, -1.0f, // Vertex 2
                1.0f, -1.0f, -1.0f, // Vertex 2
                1.0f, 1.0f, -1.0f, // Vertex 3
                1.0f, 1.0f, -1.0f, // Vertex 3
                -1.0f, 1.0f, -1.0f, // Vertex 4
                -1.0f, 1.0f, -1.0f, // Vertex 4
                -1.0f, -1.0f, -1.0f, // Vertex 1

                // Back face
                -1.0f, -1.0f, 1.0f, // Vertex 5
                1.0f, -1.0f, 1.0f, // Vertex 6
                1.0f, -1.0f, 1.0f, // Vertex 6
                1.0f, 1.0f, 1.0f, // Vertex 7
                1.0f, 1.0f, 1.0f, // Vertex 7
                -1.0f, 1.0f, 1.0f, // Vertex 8
                -1.0f, 1.0f, 1.0f, // Vertex 8
                -1.0f, -1.0f, 1.0f, // Vertex 5

                // Connecting edges
                -1.0f, -1.0f, -1.0f, // Front bottom left to Back bottom left
                -1.0f, -1.0f, 1.0f, // Front bottom right to Back bottom right
                1.0f, -1.0f, -1.0f, // Front top left to Back top left
                1.0f, -1.0f, 1.0f, // Front top right to Back top right
                1.0f, 1.0f, -1.0f, // Front top right to Back top right
                1.0f, 1.0f, 1.0f, // Front bottom right to Back bottom right
                -1.0f, 1.0f, -1.0f, // Front top left to Back top left
                -1.0f, 1.0f, 1.0f // Front bottom left to Back bottom left
        };

        glGenVertexArrays(1, &cubeOutlineVAO);
        glGenBuffers(1, &cubeOutlineVBO);
        glBindVertexArray(cubeOutlineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeOutlineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleOutlineVertices), &rectangleOutlineVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

        glBindVertexArray(0);

        float squareVertices[] = {
                1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
                -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,

                1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        };

        glGenVertexArrays(1, &squareVAO);
        glGenBuffers(1, &squareVBO);
        glBindVertexArray(squareVAO);
        glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), &squareVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));

        glBindVertexArray(0);
    }
};

#endif //SHAPES_H
