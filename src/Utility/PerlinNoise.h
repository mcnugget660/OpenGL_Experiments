#ifndef FINALDAYONEARTH_PERLINNOISE_H
#define FINALDAYONEARTH_PERLINNOISE_H

struct Octave {
    float amplitude;
    double smoothness;
};

class Texture;

class PerlinNoise {
public:

    double getPerlin(double x, double y);

    double gradientDotProduct(int x, int y, double dx, double dy);

    void createGradient(unsigned int seed);

    Texture *asTexture(int imageWidth, int imageHeight, double smoothness, float amplitude, bool repeat = false);

    Texture *asMergedTexture(const std::vector<Octave> &octaves, int imageWidth, int imageHeight, bool repeat = false);

    static unsigned int randomSeed();

private:
    static const int size = 16;
    double randomPoints[size * size * 2]{0};

    // Value is from 0 to 1
    static double lerp(double a, double b, double value);

    static double smoothStep(double a, double b, double value);

    std::vector<float> fromOctave(Octave octave, int width, int height);

};

#endif //FINALDAYONEARTH_PERLINNOISE_H
