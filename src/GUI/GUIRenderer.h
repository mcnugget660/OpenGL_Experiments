#ifndef FINALDAYONEARTH_GUIRENDERER_H
#define FINALDAYONEARTH_GUIRENDERER_H


#include <string>
#include <glm/vec3.hpp>

struct Text {
    std::string value;
    std::string font;
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


class GUIRenderer {
    static void renderText(Text &text);
    static void setUpQuad();

};


#endif //FINALDAYONEARTH_GUIRENDERER_H
