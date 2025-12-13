#include "MaterialPresets.hpp"

const std::map<std::string, MaterialPreset>& MaterialPresets::getPresets()
{
    static const std::map<std::string, MaterialPreset> presets = {
        {"Metal", {
            "Metal",
            glm::vec3(0.02f, 0.02f, 0.02f),
            glm::vec3(0.8f, 0.8f, 0.8f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            128.0f,
            1.0f,
            0.15f,
            1.0f,
            true,
            ofColor(220, 220, 220),
            "pbr"
        }},

        {"Plastic", {
            "Plastic",
            glm::vec3(0.03f, 0.03f, 0.03f),
            glm::vec3(0.45f, 0.45f, 0.45f),
            glm::vec3(0.6f, 0.6f, 0.6f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            64.0f,
            0.0f,
            0.5f,
            1.0f,
            false,
            ofColor(255, 255, 255),
            "phong"
        }},

        {"Wood", {
            "Wood",
            glm::vec3(0.08f, 0.05f, 0.02f),
            glm::vec3(0.5f, 0.34f, 0.18f),
            glm::vec3(0.01f, 0.01f, 0.01f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            4.0f,
            0.0f,
            0.8f,
            1.0f,
            true,
            ofColor(139, 90, 43),
            "lambert"
        }},

        {"Emissive", {
            "Emissive",
            glm::vec3(0.02f, 0.02f, 0.02f),
            glm::vec3(0.1f, 0.1f, 0.1f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            1.0f,
            0.0f,
            1.0f,
            1.0f,
            false,
            ofColor(255, 255, 255),
            "lambert"
        }}
    };
    return presets;
}

void MaterialPresets::applyPreset(Renderable* renderable, const std::string& presetName, ResourceManager* resourceManager)
{
    const auto& presets = getPresets();
    auto it = presets.find(presetName);
    if (it != presets.end() && renderable && renderable->material) {
        const MaterialPreset& preset = it->second;
        renderable->material->ambientReflection = preset.ambientReflection;
        renderable->material->diffuseReflection = preset.diffuseReflection;
        renderable->material->specularReflection = preset.specularReflection;
        renderable->material->emissiveReflection = preset.emissiveReflection;
        renderable->material->shininess = preset.shininess;
        renderable->material->metallic = preset.metallic;
        renderable->material->roughness = preset.roughness;
        renderable->material->ao = preset.ao;

        if (preset.overrideColor) {
            renderable->color = preset.color;
        }

        if (resourceManager && !preset.illuminationShaderName.empty()) {
            std::string vertPath = ofToDataPath("shaders/" + preset.illuminationShaderName + ".vert");
            std::string fragPath = ofToDataPath("shaders/" + preset.illuminationShaderName + ".frag");
            renderable->material->illuminationShader = &resourceManager->loadShader(vertPath, fragPath);
        }
    }
}

std::vector<std::string> MaterialPresets::getPresetNames()
{
    std::vector<std::string> names;
    const auto& presets = getPresets();

    for (const auto& pair : presets) names.push_back(pair.first);
    return names;
}