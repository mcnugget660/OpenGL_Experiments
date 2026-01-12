#ifndef FINALDAYONEARTH_GRASSCHUNK_H
#define FINALDAYONEARTH_GRASSCHUNK_H

class PerlinNoise;

class Shader;

class GrassChunk {
public:
    GrassChunk(int span, float sampleRate, bool lowLOD = false);

    ~GrassChunk();

    void render();

    void renderMultible(std::vector<glm::vec2> &chunks);

    static void setUp();

    int getBlades() const;

    int getSize() const;

private:
    unsigned int GrassVAO = -1;
    unsigned int instanceVBO = -1;
    int blades = -1;

    int size = -1;

    static unsigned int bladeVBO;
    static unsigned int bladeLowLODVBO;

    void createVAO(bool lowLOD);
};


#endif //FINALDAYONEARTH_GRASSCHUNK_H
