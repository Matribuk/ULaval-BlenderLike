#include "Core/ProceduralTexture.hpp"

ofTexture ProceduralTexture::generate(Type type, int width, int height, const ofColor& color1, const ofColor& color2)
{
    ofDisableArbTex();
    this->_pixels.allocate(width, height, OF_IMAGE_COLOR);

    switch (type) {
        case Type::PERLIN_NOISE:
            this->_generatePerlinNoise(width, height, color1, color2);
            break;
        case Type::VORONOI:
            this->_generateVoronoi(width, height, color1, color2);
            break;
        case Type::CHECKERBOARD:
            this->_generateCheckerboard(width, height, color1, color2);
            break;
        case Type::GRADIENT:
            this->_generateGradient(width, height, color1, color2);
            break;
    }

    this->_texture.allocate(width, height, GL_RGB);
    this->_texture.loadData(this->_pixels);

    return this->_texture;
}

glm::vec2 ProceduralTexture::_hash2(glm::vec2 p)
{
    p = glm::vec2(glm::dot(p, glm::vec2(127.1f, 311.7f)),
                  glm::dot(p, glm::vec2(269.5f, 183.3f)));
    glm::vec2 s = glm::sin(p) * 43758.5453123f;
    return glm::vec2(s.x - floor(s.x), s.y - floor(s.y));
}

float ProceduralTexture::_interpolate(float a, float b, float t)
{
    t = t * t * (3.0f - 2.0f * t);
    return a * (1.0f - t) + b * t;
}

float ProceduralTexture::_smoothNoise(int x, int y)
{
    int n = x + y * 57;
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

float ProceduralTexture::_perlinNoise(float x, float y)
{
    int xi = (int)floor(x);
    int yi = (int)floor(y);
    float xf = x - xi;
    float yf = y - yi;

    float v1 = this->_smoothNoise(xi, yi);
    float v2 = this->_smoothNoise(xi + 1, yi);
    float v3 = this->_smoothNoise(xi, yi + 1);
    float v4 = this->_smoothNoise(xi + 1, yi + 1);

    float i1 = this->_interpolate(v1, v2, xf);
    float i2 = this->_interpolate(v3, v4, xf);

    return this->_interpolate(i1, i2, yf);
}

void ProceduralTexture::_generatePerlinNoise(int width, int height, const ofColor& color1, const ofColor& color2)
{
    float scale = 0.05f;
    int octaves = 4;
    float persistence = 0.5f;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float value = 0.0f;
            float amplitude = 1.0f;
            float frequency = 1.0f;
            float maxValue = 0.0f;

            for (int i = 0; i < octaves; i++) {
                value += amplitude * this->_perlinNoise(x * scale * frequency, y * scale * frequency);
                maxValue += amplitude;
                amplitude *= persistence;
                frequency *= 2.0f;
            }

            value /= maxValue;
            value = (value + 1.0f) * 0.5f;

            ofColor finalColor = color1.getLerped(color2, value);
            this->_pixels.setColor(x, y, finalColor);
        }
    }
}

float ProceduralTexture::_voronoiDistance(float x, float y)
{
    float scale = 0.05f;
    glm::vec2 p = glm::vec2(x * scale, y * scale);
    glm::vec2 cellId = glm::floor(p);
    glm::vec2 localPos = glm::fract(p);

    float minDist = 1.0f;

    for (float dy = -1.0f; dy <= 1.0f; dy++) {
        for (float dx = -1.0f; dx <= 1.0f; dx++) {
            glm::vec2 neighbor = glm::vec2(dx, dy);
            glm::vec2 neighborCell = cellId + neighbor;

            glm::vec2 point = this->_hash2(neighborCell);
            glm::vec2 diff = neighbor + point - localPos;
            float dist = glm::length(diff);

            minDist = std::min(minDist, dist);
        }
    }

    return minDist;
}

void ProceduralTexture::_generateVoronoi(int width, int height, const ofColor& color1, const ofColor& color2)
{
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float dist = this->_voronoiDistance((float)x, (float)y);

            float normalizedDist = std::min(dist / 0.7f, 1.0f);

            ofColor finalColor = color1.getLerped(color2, normalizedDist);
            this->_pixels.setColor(x, y, finalColor);
        }
    }
}

void ProceduralTexture::_generateCheckerboard(int width, int height, const ofColor& color1, const ofColor& color2)
{
    int squareSize = 32;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int checkX = (x / squareSize) % 2;
            int checkY = (y / squareSize) % 2;

            ofColor finalColor = ((checkX + checkY) % 2 == 0) ? color1 : color2;
            this->_pixels.setColor(x, y, finalColor);
        }
    }
}

void ProceduralTexture::_generateGradient(int width, int height, const ofColor& color1, const ofColor& color2)
{
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float t = (float)x / (float)width;

            ofColor finalColor = color1.getLerped(color2, t);
            this->_pixels.setColor(x, y, finalColor);
        }
    }
}
