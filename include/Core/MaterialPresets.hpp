#pragma once

#include "Components/Renderable.hpp"
#include <string>
#include <map>

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
    static const std::map<std::string, MaterialPreset>& getPresets() {
        static const std::map<std::string, MaterialPreset> presets = {
            // 1. Metal - High specular, low diffuse, high shininess
            {"Metal", {
                "Metal",
                glm::vec3(0.1f, 0.1f, 0.1f),    // Low ambient
                glm::vec3(0.3f, 0.3f, 0.3f),    // Low diffuse
                glm::vec3(0.9f, 0.9f, 0.9f),    // High specular (shiny)
                glm::vec3(0.0f, 0.0f, 0.0f),    // No emissive
                128.0f                           // Very high shininess
            }},

            // 2. Plastic - Moderate specular, high diffuse, medium shininess
            {"Plastic", {
                "Plastic",
                glm::vec3(0.2f, 0.2f, 0.2f),    // Moderate ambient
                glm::vec3(0.7f, 0.7f, 0.7f),    // High diffuse
                glm::vec3(0.5f, 0.5f, 0.5f),    // Moderate specular
                glm::vec3(0.0f, 0.0f, 0.0f),    // No emissive
                32.0f                            // Medium shininess
            }},

            // 3. Wood - Low specular, high diffuse, low shininess
            {"Wood", {
                "Wood",
                glm::vec3(0.3f, 0.2f, 0.1f),    // Warm ambient (brownish)
                glm::vec3(0.6f, 0.4f, 0.2f),    // Warm diffuse (brownish)
                glm::vec3(0.1f, 0.1f, 0.1f),    // Very low specular (matte)
                glm::vec3(0.0f, 0.0f, 0.0f),    // No emissive
                4.0f                             // Very low shininess
            }},

            // 4. Emissive - Glowing material with emissive component
            {"Emissive", {
                "Emissive",
                glm::vec3(0.1f, 0.1f, 0.1f),    // Low ambient
                glm::vec3(0.3f, 0.3f, 0.3f),    // Low diffuse
                glm::vec3(0.0f, 0.0f, 0.0f),    // No specular
                glm::vec3(0.5f, 0.5f, 0.5f),    // High emissive (glowing)
                1.0f                             // Minimal shininess
            }},

            // Additional presets for variety
            {"Gold", {
                "Gold",
                glm::vec3(0.24725f, 0.1995f, 0.0745f),
                glm::vec3(0.75164f, 0.60648f, 0.22648f),
                glm::vec3(0.628281f, 0.555802f, 0.366065f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                51.2f
            }},

            {"Silver", {
                "Silver",
                glm::vec3(0.19225f, 0.19225f, 0.19225f),
                glm::vec3(0.50754f, 0.50754f, 0.50754f),
                glm::vec3(0.508273f, 0.508273f, 0.508273f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                51.2f
            }},

            {"Rubber", {
                "Rubber",
                glm::vec3(0.02f, 0.02f, 0.02f),
                glm::vec3(0.5f, 0.5f, 0.5f),
                glm::vec3(0.1f, 0.1f, 0.1f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                10.0f
            }}
        };
        return presets;
    }

    static void applyPreset(Material* material, const std::string& presetName) {
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

    static std::vector<std::string> getPresetNames() {
        std::vector<std::string> names;
        const auto& presets = getPresets();
        for (const auto& pair : presets) {
            names.push_back(pair.first);
        }
        return names;
    }
};
