#ifndef VALUENOISE2D_H
#define VALUENOISE2D_H

#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

class ValueNoise2D {
private:
    std::vector<float> grid;
    int gridSize;
    unsigned int seed;

    float fade(float t) const {
        return t * t * (3 - 2 * t); // Simple smoothstep
    }

    float lerp(float a, float b, float t) const {
        return a + t * (b - a);
    }

    float randomValue(int x, int y) const {
        // Deterministic pseudo-random per cell
        int n = x + y * 129 + seed * 673;
        n = (n << 13) ^ n;
        return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
    }

public:
    ValueNoise2D(unsigned int seed = 0) : seed(seed) {}

    float noise(float x, float y) const {
        int xi = static_cast<int>(floor(x));
        int yi = static_cast<int>(floor(y));
        float xf = x - xi;
        float yf = y - yi;

        float v00 = randomValue(xi,     yi);
        float v10 = randomValue(xi + 1, yi);
        float v01 = randomValue(xi,     yi + 1);
        float v11 = randomValue(xi + 1, yi + 1);

        float u = fade(xf);
        float v = fade(yf);

        float x1 = lerp(v00, v10, u);
        float x2 = lerp(v01, v11, u);
        return lerp(x1, x2, v); // Output in [-1, 1]
    }
};

#endif