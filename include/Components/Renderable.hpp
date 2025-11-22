#pragma once

#include <ofMain.h>

struct Material {
    // Shader pipeline: shaders are applied in order
    // First shader is typically illumination, subsequent ones are effects
    std::vector<ofShader*> shaderPipeline;

    // Legacy compatibility
    ofShader* shader = nullptr;              // Effect shader (rainbow, etc.)
    ofShader* illuminationShader = nullptr;  // Illumination shader (lambert, phong, toon)

    ofTexture* texture = nullptr;

    float scale = 5.0f;
    float octaves = 4.0f;
    float persistence = 0.5f;
    float borderWidth = 0.05f;

    glm::vec3 lightPosition = glm::vec3(5.0f, 5.0f, 5.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float lightIntensity = 1.0f;
    glm::vec3 ambientColor = glm::vec3(0.2f, 0.2f, 0.2f);
    float shininess = 32.0f;

    // Material reflection components
    glm::vec3 ambientReflection = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 diffuseReflection = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 specularReflection = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 emissiveReflection = glm::vec3(0.0f, 0.0f, 0.0f);
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

    // Copy constructor - deep copy of Material
    Renderable(const Renderable& other)
        : mesh(other.mesh), color(other.color), visible(other.visible),
          showOutline(other.showOutline), isPrimitive(other.isPrimitive) {
        if (other.material) {
            material = new Material(*other.material);  // Deep copy
        } else {
            material = nullptr;
        }
    }

    // Copy assignment operator - deep copy of Material
    Renderable& operator=(const Renderable& other) {
        if (this != &other) {
            mesh = other.mesh;
            color = other.color;
            visible = other.visible;
            showOutline = other.showOutline;
            isPrimitive = other.isPrimitive;

            // Delete old material
            if (material) {
                delete material;
            }

            // Deep copy new material
            if (other.material) {
                material = new Material(*other.material);
            } else {
                material = nullptr;
            }
        }
        return *this;
    }

    // Destructor - clean up Material
    ~Renderable() {
        if (material) {
            delete material;
            material = nullptr;
        }
    }
};

