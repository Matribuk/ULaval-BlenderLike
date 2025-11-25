#pragma once
#include <ofMain.h>
#include "Core/Cubemap.hpp"

struct Material {
    std::vector<ofShader*> shaderPipeline;
    std::vector<ofShader*> effects;

    ofShader* shader = nullptr;
    ofShader* illuminationShader = nullptr;

    ofTexture* texture = nullptr;
    ofTexture* normalMap = nullptr;
    ofTexture* heightMap = nullptr;

    float scale = 5.0f;
    float octaves = 4.0f;
    float persistence = 0.5f;
    float borderWidth = 0.05f;

    glm::vec3 lightPosition = glm::vec3(5.0f, 5.0f, 5.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float lightIntensity = 1.0f;
    glm::vec3 ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
    float shininess = 32.0f;

    glm::vec3 ambientReflection = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 diffuseReflection = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 specularReflection = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 emissiveReflection = glm::vec3(0.0f, 0.0f, 0.0f);

    float reflectivity = 0.0f;
    glm::vec3 reflectionTint = glm::vec3(1.0f, 1.0f, 1.0f);
    float normalStrength = 1.0f;

    float displacementStrength = 0.5f;
    bool useDisplacement = false;

    Cubemap* dynamicCubemap = nullptr;
    bool useDynamicCubemap = false;
    int dynamicCubemapSize = 256;

    bool isLightSource = false;
};


struct Renderable {

    ofMesh mesh;
    ofColor color{255, 255, 255};
    bool visible{true};
    bool showOutline{false};
    bool isPrimitive{false};

    Material* material{nullptr};

    Renderable() = default;

    Renderable(
        const ofMesh& m,
        const ofColor& c = ofColor(255, 255, 255),
        bool v = true,
        ofShader* s = nullptr,
        ofTexture* t = nullptr,
        bool primitive = false
    ) :
        mesh(m),
        color(c),
        visible(v),
        isPrimitive(primitive)
    {

        material = new Material();
        material->shader = s;
        if (s) material->effects.push_back(s);
        material->texture = t;
    }

    Renderable(const Renderable& other)
    :
        mesh(other.mesh),
        color(other.color),
        visible(other.visible),
        showOutline(other.showOutline),
        isPrimitive(other.isPrimitive)
    {
        if (other.material) material = new Material(*other.material);
        else material = nullptr;
    }

    Renderable& operator=(const Renderable& other)
    {
        if (this != &other) {
            mesh = other.mesh;
            color = other.color;
            visible = other.visible;
            showOutline = other.showOutline;
            isPrimitive = other.isPrimitive;

            if (material) delete material;

            material = other.material ? new Material(*other.material) : nullptr;
        }
        return *this;
    }

    ~Renderable() { delete material; }
};
