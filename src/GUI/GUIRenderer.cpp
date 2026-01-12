#include "GUIRenderer.h"
#include "FontManager.h"
#include "../Utility/ResourceLoader.h"

void GUIRenderer::renderText(Text &text) {
    Shader &textShader = *ResourceLoader::getShader("GUI/text");
//Screen width and height don't have to be the real values and are used in positioning the text

        float scale = text.scale;
        int originX = text.relativeX;
        int originY = text.relativeY;

        Font &font = ResourceLoader::getFontManager().getFont(text.font);

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

        //glBindVertexArray(quadVAO);

        std::unordered_map<char, Character> &characters = font.characters;
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
            //glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);


            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);



}

/*
void GUIRenderer::setUpQuad() {
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 6, NULL, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);

    glBindVertexArray(0);
}
*/
