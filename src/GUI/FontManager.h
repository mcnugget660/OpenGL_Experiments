#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <glm/vec2.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>

#include <ft2build.h>

#include <utility>
#include FT_FREETYPE_H


using namespace std;

struct Character {
    glm::vec2 textureMin;
    glm::vec2 textureMax;
    int advance;
    glm::vec2 distFromOrigin;
    int w;
    int h;

    Character(glm::vec2 texMin, glm::vec2 texMax, int ad, glm::vec2 OD, int w, int h) : textureMin(texMin),
        textureMax(texMax), advance(ad), distFromOrigin(OD), w(w), h(h) {
    }
};

struct Font {
    unordered_map<char, Character> characters;
    unsigned int textureId;
    int width;
    int height;
};

class FontManager {
public:
    explicit FontManager(string fontDirectory) : directory(std::move(fontDirectory)) {
        loadFont("default.ttf");
    }

    void loadFont(const string& font) {
        FT_Library ft;

        if (FT_Init_FreeType(&ft)) {
            cout << "ERROR::FREETYPE: Could not init FreeType Library" << endl;
            return;
        }

        FT_Face face;
        if (FT_New_Face(ft, (directory + font).c_str(), 0, &face)) {
            cout << "ERROR::FREETYPE: Failed to load font" << endl;
            return;
        }

        unordered_map<char, Character> map;

        // m signifies distance in memory, p signifies distance in pixels
        int m_unit = sizeof(unsigned char);

        int totalCharacters = 128;
        int normalCharacterHeight = 48;

        int imageWidth_p = normalCharacterHeight * 16; // Sixteen characters on each row
        int imageHeight_p = imageWidth_p / 2;

        std::vector<unsigned char> buffer(imageWidth_p * imageHeight_p);

        int currentWidth_p = 0;
        int currentHeight_p = 0;
        int rowHeight_p = 0; // Maximum Height for the current row
        int index = 0; // Index of the top left corner for the character

        int error = FT_Set_Pixel_Sizes(face, 0, normalCharacterHeight);
        if (error)
            cout << "FT Error in setting pixel size : " << "\n";

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        FT_GlyphSlot slot = face->glyph;

        for (unsigned char c = 32; c < totalCharacters; c++) {
            if (FT_Get_Char_Index(face, c) == 0) // Is not part of the font
                continue;

            // load character glyph
            error = FT_Load_Char(face, c, FT_LOAD_RENDER);
            if (error)
                cout << "FT Error in loading character : " << "\n";

            if (c == ' ') {
                map.insert(std::make_pair(c, Character{{}, {}, (int) slot->advance.x >> 6, {}, 0, 0}));
                continue;
            }

            error = FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);

            if (error)
                cout << "FT Error in rendering glyph : " <<"\n";

            int characterWidth_p = slot->bitmap.width;
            int characterHeight_p = slot->bitmap.rows;

            // Go to next row
            if (currentWidth_p + characterWidth_p > imageWidth_p) {
                index = imageWidth_p * (currentHeight_p + rowHeight_p);
                currentHeight_p += rowHeight_p;
                currentWidth_p = 0;
                rowHeight_p = 0;
            }

            //This will ensure following characters will not overlap : Checked after row placement is confirmed
            if (characterHeight_p > rowHeight_p)
                rowHeight_p = characterHeight_p;

            //If the character will not fit in the texture then expand the texture
            if (imageHeight_p < currentHeight_p + rowHeight_p) {
                imageHeight_p = rowHeight_p + currentHeight_p;
                buffer.resize(imageHeight_p * imageWidth_p); // In elements
            }

            //Copy character : i - current row of pixels
            for (int i = 0; i < slot->bitmap.rows; i++)
                for (int o = 0; o < slot->bitmap.width; o++)
                    buffer[index + i * imageWidth_p + o] = slot->bitmap.buffer[o + characterWidth_p * i];


            //Set texture coordinates : Topleft is 0,0 in this case
            float minX = (currentWidth_p + 0.5) / imageWidth_p;
            float maxX = minX + (characterWidth_p - 0.5) / imageWidth_p;
            //Height is unknown so is updated later
            float minY = currentHeight_p + 0.5;
            float maxY = minY + (characterHeight_p - 0.5);
            Character f(
                {minX, minY}, {maxX, maxY},
                static_cast<int>(slot->advance.x >> 6), // From 1/64 of pixels to pixels
                {slot->bitmap_left, characterHeight_p - slot->bitmap_top},
                characterWidth_p, characterHeight_p
            );

            map.insert(std::make_pair(c, f));

            //Next character
            currentWidth_p += characterWidth_p;
            index += characterWidth_p;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0,GL_RED, imageWidth_p * m_unit, imageHeight_p * m_unit, 0,GL_RED,
                     GL_UNSIGNED_BYTE, &buffer[0]
        );

        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        string fontName = font.substr(0, font.find("."));

        for (auto& [key, val]: map) {
            val.textureMin.y /= (float) imageHeight_p;
            val.textureMax.y /= (float) imageHeight_p;
        }

        fonts[font] = Font{map, texture, imageWidth_p, imageHeight_p};
    }

    void freeFonts(){
        for(auto& f : fonts) {
            glDeleteTextures(1, &f.second.textureId);
        }
    }

    Font& getFont(string fontName) {
        if (fonts.count(fontName) == 0)
            return fonts.begin()->second; // default font is first
        return fonts[fontName];
    }

private:
    string directory;
    unordered_map<string, Font> fonts;
};

#endif //FONTMANAGER_H
