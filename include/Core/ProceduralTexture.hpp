#pragma once

#include "ofMain.h"
#include <string>

class ProceduralTexture {
    public:
        enum class Type {
            PERLIN_NOISE,
            VORONOI,
            CHECKERBOARD,
            GRADIENT
        };

        ProceduralTexture();
        ~ProceduralTexture() = default;

        ofTexture generate(Type type, int width, int height, const ofColor& color1, const ofColor& color2);

        const ofTexture& getTexture() const { return _texture; }

    private:
        ofTexture _texture;
        ofPixels _pixels;

        void _generatePerlinNoise(int width, int height, const ofColor& color1, const ofColor& color2);
        void _generateVoronoi(int width, int height, const ofColor& color1, const ofColor& color2);
        void _generateCheckerboard(int width, int height, const ofColor& color1, const ofColor& color2);
        void _generateGradient(int width, int height, const ofColor& color1, const ofColor& color2);

        float _perlinNoise(float x, float y);
        float _smoothNoise(int x, int y);
        float _interpolate(float a, float b, float t);
        glm::vec2 _hash2(glm::vec2 p);
        float _voronoiDistance(float x, float y);
};
