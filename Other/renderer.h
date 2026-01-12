#ifndef RENDERER_H
#define RENDERER_H

#include <GLFW/glfw3.h>

#include "GameObject.h"
#include "Gui/Gui.h"
#include "rendering/FontManager.h"
#include "rendering/shader.h"
#include "rendering/skybox.h"

struct Text {
    string value;
    string font;
    glm::vec3 color;
    float scale;
    float thickness;
    float softness;
    float relativeX; // 0 to 1000
    float relativeY; // 0 to 1000

    bool outline = false;

    bool glow = false;

    bool shadow = false;
};

class Renderer {
public:
    FontManager fontManager;

    glm::vec3 sun;

    bool showAllBounds = false;

    Renderer() : basicShader("basic.vert", "basic.frag"), lightingShader("model.vert", "model.frag"),
                 lightSourceShader("lightSource.vert", "lightSource.frag"),
                 skyboxShader("skybox.vert", "skybox.frag"), guiShader("gui.vert", "gui.frag"),
                 textShader("text.vert", "text.frag"),
                 fontManager("/home/conrad/c++Projects/OpenGl Render/res/fonts/") {
        fontManager.loadFont("BitterPro-Black.ttf");
        setUpQuad();
    }

    void RenderObjects(vector<GameObject *> &objects, glm::mat<4, 4, float> projection, glm::mat4 view,
                       GameObject *selectedObject) {
        lightingShader.use();

        lightingShader.setFloat("material.shininess", 8.0f);

        lightingShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
        lightingShader.setVec3("dirLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("dirLight.direction", sun);
        //cos(glfwGetTime()), sin(glfwGetTime()), cos(glfwGetTime())

        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);


        for (GameObject *object: objects) {
            if (selectedObject == object)
                continue;
            object->Render(lightingShader);
        }

        //Bounds
        basicShader.use();

        basicShader.setMat4("projection", projection);
        basicShader.setMat4("view", view);

        basicShader.setVec3("Color", glm::vec3(255, 0, 0));

        for (GameObject *object: objects) {
            if (selectedObject == object)
                continue;
            if (object->showBounds)
                object->RenderBounds(basicShader, shapeRenderer);
            else if (showAllBounds)
                object->RenderBounds(basicShader, shapeRenderer);
        }

        //SelectedObject
        if (selectedObject != nullptr) {
            basicShader.setVec3("Color", glm::vec3(0, 255, 0));
            selectedObject->Render(basicShader);
        }
    }

    std::string glErrorToString(GLenum error) {
        switch (error) {
            case GL_NO_ERROR:
                return "No error";
            case GL_INVALID_ENUM:
                return "Invalid enum";
            case GL_INVALID_VALUE:
                return "Invalid value";
            case GL_INVALID_OPERATION:
                return "Invalid operation";
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                return "Invalid framebuffer operation";
            case GL_OUT_OF_MEMORY:
                return "Out of memory";
            default:
                return "Unknown error";
        }
    }

    void RenderGui(Gui &gui) {
        glClear(GL_DEPTH_BUFFER_BIT); // Clear depth information

        guiShader.use();
        gui.draw(guiShader);
    }


    void RenderSkybox(Camera &camera, glm::mat<4, 4, float> projection, glm::mat4 view) {
        // change depth function so depth test passes when values are equal to depth buffer's content
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();

        // remove translation from the view matrix
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
        skyboxShader.setMat4("projection", projection);

        skybox.Draw();
        glDepthFunc(GL_LESS);
    }

    void renderLine(glm::vec3 point1, glm::vec3 point2, glm::mat<4, 4, float> projection, glm::mat4 view,
                    glm::vec3 color) {
        basicShader.use();

        glm::mat4 model_mat = glm::mat4(1.0f);

        basicShader.setMat4("model", model_mat);

        basicShader.setMat4("projection", projection);
        basicShader.setMat4("view", view);

        basicShader.setVec3("Color", color);
        shapeRenderer.drawLine(point1, point2);
    }

    //Screen width and height dont have to be the real values and are used in positioning the text
    void renderText(Text &text) {
        float scale = text.scale;
        int originX = text.relativeX;
        int originY = text.relativeY;

        Font &font = fontManager.getFont(text.font);

        textShader.use();

        glm::mat4 projection = glm::ortho(0.0f, 1000.0f, 0.0f, 1000.0f);
        textShader.setMat4("projection", projection);
        textShader.setVec3("color", text.color);
        textShader.setFloat("softness", text.softness);
        textShader.setFloat("thickness", text.thickness);
        textShader.setBool("outline", text.outline);
        textShader.setBool("glow", text.glow);

        textShader.setBool("shadow", text.shadow);
        textShader.setVec2("shadowOffset", glm::vec2(-2.0f / font.width, -2.0f / font.height));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, font.textureId);

        glBindVertexArray(quadVAO);

        map<char, Character> &characters = font.characters;
        std::string::const_iterator c;
        for (c = text.value.begin(); c != text.value.end(); c++) {
            Character &ch = characters.at(*c);

            if (*c == ' ') {
                originX += ch.advance * scale;
                continue;
            }

            float h = ch.h * scale;
            float w = ch.w * scale;
            float x = originX + ch.distFromOrigin.x * scale;
            float y = originY - (ch.distFromOrigin.y * scale);
            float vertices[6][4] = {
                    {x,     y + h, ch.textureMin.x, ch.textureMin.y}, // Top left
                    {x,     y,     ch.textureMin.x, ch.textureMax.y}, // Bottom left
                    {x + w, y,     ch.textureMax.x, ch.textureMax.y}, // Bottom right

                    {x,     y + h, ch.textureMin.x, ch.textureMin.y}, // Top left
                    {x + w, y,     ch.textureMax.x, ch.textureMax.y}, // Bottom right
                    {x + w, y + h, ch.textureMax.x, ch.textureMin.y} // Top right
            };

            //Shift origin for next character
            originX += ch.advance * scale;

            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);
    }

private:
    Shader lightingShader;

    Shader lightSourceShader;

    Shader skyboxShader;

    Shader basicShader;

    Shader guiShader;

    Shader textShader;

    Skybox skybox;

    ShapeRenderer shapeRenderer;

    unsigned int quadVAO;
    unsigned int quadVBO;

    void setUpQuad() {
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 6, NULL, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);

        glBindVertexArray(0);
    }
};

#endif //RENDERER_H
