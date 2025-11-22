#pragma once

#include <ofMain.h>

struct Material {
    std::vector<ofShader*> shaderPipeline;

    ofShader* shader = nullptr;
    ofShader* illuminationShader = nullptr;

    ofTexture* texture = nullptr;

    float scale = 5.0f;
    float octaves = 4.0f;
    float persistence = 0.5f;
    float borderWidth = 0.05f;

    glm::vec3 lightPosition = glm::vec3(5.0f, 5.0f, 5.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float lightIntensity = 1.0f;
    glm::vec3 ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
    float shininess = 32.0f;

    glm::vec3 ambientReflection = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 diffuseReflection = glm::vec3(0.55f, 0.55f, 0.55f);
    glm::vec3 specularReflection = glm::vec3(0.70f, 0.70f, 0.70f);
    glm::vec3 emissiveReflection = glm::vec3(0.0f, 0.0f, 0.0f);

    bool isLightSource = false;
};

struct Renderable {
    ofMesh mesh;
    ofColor color{255, 255, 255};
    bool visible{true};
    Material* material{nullptr};

    bool showOutline{false};
    bool isPrimitive{false};

    Renderable() = default;

    Renderable(const ofMesh& m, const ofColor& c = ofColor(255, 255, 255), bool v = true, ofShader* s = nullptr, ofTexture* t = nullptr, bool primitive = false)
        : mesh(m), color(c), visible(v), isPrimitive(primitive) {
        material = new Material();
        material->shader = s;
        material->texture = t;
    }

    Renderable(const Renderable& other)
        : mesh(other.mesh), color(other.color), visible(other.visible),
          showOutline(other.showOutline), isPrimitive(other.isPrimitive) {
        if (other.material) {
            material = new Material(*other.material);
        } else {
            material = nullptr;
        }
    }

    Renderable& operator=(const Renderable& other) {
        if (this != &other) {
            mesh = other.mesh;
            color = other.color;
            visible = other.visible;
            showOutline = other.showOutline;
            isPrimitive = other.isPrimitive;

            if (material) {
                delete material;
            }

            if (other.material) {
                material = new Material(*other.material);
            } else {
                material = nullptr;
            }
        }
        return *this;
    }

    ~Renderable() {
        if (material) {
            delete material;
            material = nullptr;
        }
    }
};

