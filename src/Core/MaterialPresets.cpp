#include "MaterialPresets.hpp"

const std::map<std::string, MaterialPreset>& MaterialPresets::getPresets()
{
    static const std::map<std::string, MaterialPreset> presets = {
        {"Metal", {
            "Metal",
            glm::vec3(0.02f, 0.02f, 0.02f),
            glm::vec3(0.04f, 0.04f, 0.04f),
            glm::vec3(0.9f, 0.9f, 0.9f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            64.0f
        }},

        {"Plastic", {
            "Plastic",
            glm::vec3(0.03f, 0.03f, 0.03f),
            glm::vec3(0.45f, 0.45f, 0.45f),
            glm::vec3(0.6f, 0.6f, 0.6f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            64.0f
        }},

        {"Wood", {
            "Wood",
            glm::vec3(0.08f, 0.05f, 0.02f),
            glm::vec3(0.5f, 0.34f, 0.18f),
            glm::vec3(0.01f, 0.01f, 0.01f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            4.0f
        }},

        {"Emissive", {
            "Emissive",
            glm::vec3(0.02f, 0.02f, 0.02f),
            glm::vec3(0.1f, 0.1f, 0.1f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            1.0f
        }}
    };
    return presets;
}

void MaterialPresets::applyPreset(Material* material, const std::string& presetName)
{
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

std::vector<std::string> MaterialPresets::getPresetNames()
{
    std::vector<std::string> names;
    const auto& presets = getPresets();

    for (const auto& pair : presets) names.push_back(pair.first);
    return names;
}