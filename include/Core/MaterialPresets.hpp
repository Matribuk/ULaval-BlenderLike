#pragma once

#include "Components/Renderable.hpp"
#include "Manager/ResourceManager.hpp"
#include <string>
#include <map>
#include <vector>

struct MaterialPreset {
    std::string name;
    glm::vec3 ambientReflection;
    glm::vec3 diffuseReflection;
    glm::vec3 specularReflection;
    glm::vec3 emissiveReflection;
    float shininess;
    float metallic;
    float roughness;
    float ao;
    bool overrideColor;
    ofColor color;
    std::string illuminationShaderName;
};

class MaterialPresets {
    public:
        static const std::map<std::string, MaterialPreset>& getPresets();
        static void applyPreset(Renderable* renderable, const std::string& presetName, ResourceManager* resourceManager = nullptr);
        static std::vector<std::string> getPresetNames();
};