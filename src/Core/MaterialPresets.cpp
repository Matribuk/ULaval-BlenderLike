#include "Core/MaterialPresets.hpp"

const std::map<std::string, MaterialPreset>& MaterialPresets::getPresets() {
    static const std::map<std::string, MaterialPreset> presets = {
        {"Plastic", {
            "Plastic",
            glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3(0.55f, 0.55f, 0.55f),
            glm::vec3(0.70f, 0.70f, 0.70f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            32.0f
        }},
        {"Wood", {
            "Wood",
            glm::vec3(0.3f, 0.3f, 0.3f),
            glm::vec3(0.64f, 0.64f, 0.64f),
            glm::vec3(0.15f, 0.15f, 0.15f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            4.0f
        }},
        {"Metal", {
            "Metal",
            glm::vec3(0.1f, 0.1f, 0.1f),
            glm::vec3(0.19f, 0.19f, 0.18f),
            glm::vec3(0.77f, 0.77f, 0.77f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            89.6f
        }},
        {"Emissive", {
            "Emissive",
            glm::vec3(0.05f, 0.05f, 0.05f),
            glm::vec3(0.10f, 0.10f, 0.10f),
            glm::vec3(0.05f, 0.05f, 0.05f),
            glm::vec3(0.80f, 0.80f, 0.80f),
            1.0f
        }}
    };
    return presets;
}

void MaterialPresets::applyPreset(Material* material, const std::string& presetName) {
    const auto& presets = getPresets();
    auto it = presets.find(presetName);
    if (it != presets.end() && material) {
        const MaterialPreset& preset = it->second;
        material->ambientReflection = preset.ambientReflection;
        material->diffuseReflection = preset.diffuseReflection;
        material->specularReflection = preset.specularReflection;
        material->emissiveReflection = preset.emissiveReflection;
        material->shininess = preset.shininess;
    }
}

std::vector<std::string> MaterialPresets::getPresetNames() {
    std::vector<std::string> names;
    const auto& presets = getPresets();
    for (const auto& pair : presets) {
        names.push_back(pair.first);
    }
    return names;
}
