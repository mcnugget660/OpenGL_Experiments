#include <cmath>
#include <random>
#include <iostream>

#include "PerlinNoise.h"
#include "glad/glad.h"
#include "../Rendering/Texture.h"
#include "TextureUtil.h"

Texture *PerlinNoise::asTexture(int imageWidth, int imageHeight, double smoothness, float amplitude, bool repeat) {
    if (imageWidth <= 0 || imageHeight <= 0) {
        std::cout << "Tried to create impossible texture!!!!!!!!!!!!!!!!!!!!!!!1";
        return nullptr;
    }

    return TextureUtil::floatArray2DToTexture(&fromOctave({amplitude, smoothness}, imageWidth, imageHeight)[0], imageWidth, false, repeat);
}

void PerlinNoise::createGradient(unsigned int seed) {
    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> distribution(-1.0, 1.0);

    for (int i = 0; i < 16; i++) {
        for (int y = 0; y < 16; y++) {
            randomPoints[i * 32 + y * 2] = distribution(generator);
            randomPoints[i * 32 + y * 2 + 1] = distribution(generator);
        }
    }
}

double PerlinNoise::lerp(double a, double b, double value) {
    return a + (b - a) * value;
}

double PerlinNoise::smoothStep(double a, double b, double value) {
    // 3x^2 - 2x^3
    return a + value * value * (3 - 2 * value) * (b - a);
}

double PerlinNoise::getPerlin(double x, double y) {
    x += 0.01;
    y += 0.01;

    // Flips adjacent sections for a smooth transition
    x = ((int) x / 16) % 2 == 1 ? 16 - fmod(x, 16) : fmod(x, 16);
    y = ((int) y / 16) % 2 == 1 ? 16 - fmod(y, 16) : fmod(y, 16);

    int nx = x;
    int ny = y;

    double rx = x - nx;
    double ry = y - ny;

    //Top points
    double a = gradientDotProduct(nx, ny, rx, ry);
    double b = gradientDotProduct(nx + 1, ny, rx - 1, ry);

    //Bottom points
    double c = gradientDotProduct(nx, ny + 1, rx, ry - 1);
    double d = gradientDotProduct(nx + 1, ny + 1, rx - 1, ry - 1);

    double average1 = smoothStep(a, b, rx);
    double average2 = smoothStep(c, d, rx);

    return std::max(0.0, std::min(1.0, (smoothStep(average1, average2, ry) + 0.5)));
}

double PerlinNoise::gradientDotProduct(int x, int y, double dx, double dy) {
    double gx = randomPoints[x * 32 + y * 2];
    double gy = randomPoints[x * 32 + y * 2 + 1]; // x and y values are stored in sequence

    //Return dot product of the gradient vector and the vector to the coordinate to get similarity
    return gx * dx + gy * dy;
}

unsigned int PerlinNoise::randomSeed() {
    std::random_device rd;
    return rd();
}

Texture *PerlinNoise::asMergedTexture(const std::vector<Octave> &octaves, int imageWidth, int imageHeight, bool repeat) {
    std::vector<std::vector<float>> data;
    for (Octave octave: octaves)
        data.push_back(fromOctave(octave, imageWidth, imageHeight));
    for (int i = 1; i < data.size(); ++i) {
        for (int j = 0; j < imageWidth * imageHeight; ++j) {
            data[0][j] += data[i][j];
        }
    }

    return TextureUtil::floatArray2DToTexture(&data[0][0], imageWidth, false, repeat);
}

std::vector<float> PerlinNoise::fromOctave(Octave octave, int width, int height) {
    std::vector<float> data(width * height);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            double dx = x / octave.smoothness;
            double dy = y / octave.smoothness;
            data[x * width + y] = (float) getPerlin(dx, dy) * octave.amplitude;
        }
    }

    return data;
}
