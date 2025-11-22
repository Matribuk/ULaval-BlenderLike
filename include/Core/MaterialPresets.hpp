#pragma once

#include "Components/Renderable.hpp"
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
};

class MaterialPresets {
    public:
        static const std::map<std::string, MaterialPreset>& getPresets();
        static void applyPreset(Material* material, const std::string& presetName);
        static std::vector<std::string> getPresetNames();
};
