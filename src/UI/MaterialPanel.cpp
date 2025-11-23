#include "UI/MaterialPanel.hpp"

MaterialPanel::MaterialPanel(ComponentRegistry& componentRegistry, SelectionSystem& selectionSystem, ResourceManager& resourceManager, PrimitiveSystem& primitiveSystem)
    : _componentRegistry(componentRegistry), _selectionSystem(selectionSystem), _resourceManager(resourceManager), _primitiveSystem(primitiveSystem){}

bool MaterialPanel::_checkAllEntitiesHaveSameVisibility(const std::set<EntityID>& entities, bool& outVisibility) const
{
    if (entities.empty()) return false;

    bool first = true;
    bool referenceVisibility = false;

    for (EntityID id : entities) {
        Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(id);
        if (!renderable) return false;

        if (first) {
            referenceVisibility = renderable->visible;
            first = false;
        } else {
            if (renderable->visible != referenceVisibility)
                return false;
        }
    }

    outVisibility = referenceVisibility;
    return true;
}

void MaterialPanel::render()
{
    const std::set<EntityID>& selectedEntities = this->_selectionSystem.getSelectedEntities();

    if (selectedEntities.empty()) {
        this->_prevSelectedEntities.clear();
        return;
    }

    bool allHaveRenderable = true;
    for (EntityID id : selectedEntities) {
        if (!this->_componentRegistry.getComponent<Renderable>(id))
            allHaveRenderable = false;
    }

    if (!allHaveRenderable) {
        if (selectedEntities.size() > 1) {
            ImGui::Text("Some selected entities don't have Renderable component");
        } else {
            ImGui::Button("Add Renderable Component");
            if (ImGui::IsItemClicked()) this->_addMaterialComponent(*selectedEntities.begin());
        }
        this->_prevSelectedEntities.clear();
        return;
    }

    EntityID primaryEntity = this->_selectionSystem.getSelectedEntity();
    Renderable* primaryRenderable = this->_componentRegistry.getComponent<Renderable>(primaryEntity);

    if (!primaryRenderable) {
        this->_prevSelectedEntities.clear();
        return;
    }

    if (this->_prevSelectedEntities != selectedEntities)
        this->_prevSelectedEntities = selectedEntities;

    this->_renderVisibilityControl(selectedEntities, primaryRenderable);

    if (selectedEntities.size() > 1)
        ImGui::Text("(%zu entities selected)", selectedEntities.size());

    if (primaryRenderable->material) {
        ImGui::Text("Material:");
        this->_renderShaderSection(primaryEntity, selectedEntities, primaryRenderable);
        this->_renderTextureSection(primaryEntity, primaryRenderable);
        this->_renderMeshSection(primaryEntity, primaryRenderable);

        ImGui::Separator();
        this->_renderLightingParameters(selectedEntities, primaryRenderable);

        ImGui::Separator();
        this->_renderReliefMappingSection(primaryEntity, selectedEntities, primaryRenderable);
    }
}

void MaterialPanel::_addMaterialComponent(EntityID entityId)
{
    if (entityId == INVALID_ENTITY) return;

    this->_componentRegistry.registerComponent<Renderable>(entityId, Renderable(ofMesh(), ofColor::white));
}

void MaterialPanel::_loadShaders(const std::set<EntityID>& selectedEntities, Renderable* primaryRenderable)
{
    if (ImGui::Button("Load Shaders"))
        ImGui::OpenPopup("LoadShadersPopup");

    if (ImGui::BeginPopup("LoadShadersPopup")) {
        std::filesystem::path shaderDir = std::filesystem::path(ofToDataPath("shaders"));
        std::vector<std::string> names;
        if (std::filesystem::exists(shaderDir) && std::filesystem::is_directory(shaderDir)) {
            for (std::filesystem::directory_entry entry : std::filesystem::directory_iterator(shaderDir)) {
                if (!entry.is_regular_file()) continue;
                std::string ext = entry.path().extension().string();
                if (ext == ".vert" || ext == ".frag") {
                    names.push_back(entry.path().stem().string());
                }
            }
        }
        std::sort(names.begin(), names.end());
        names.erase(std::unique(names.begin(), names.end()), names.end());

        if (names.empty()) {
            ImGui::TextDisabled("No shaders found in data/shaders");
        } else {
            for (const std::string &n : names) {
                if (n == "skycube") continue;

                if (ImGui::Selectable(n.c_str())) {
                    std::filesystem::path vert = shaderDir / (n + ".vert");
                    std::filesystem::path frag = shaderDir / (n + ".frag");
                    if (std::filesystem::exists(vert) && std::filesystem::exists(frag)) {
                        ofShader& loaded = this->_resourceManager.loadShader(vert.string(), frag.string());

                        for (EntityID id : selectedEntities) {
                            Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(id);
                            if (renderable && renderable->material) {
                                renderable->material->shader = &loaded;
                            }
                        }
                    }
                    ImGui::CloseCurrentPopup();
                }
            }
        }

        ImGui::EndPopup();
    }
}

void MaterialPanel::_loadFile(EntityID entityId, Renderable* primaryRenderable, std::string type)
{
    std::string title = type.compare("MESH") == 0 ? "Load mesh" : "Load texture";
    if (ImGui::Button(title.c_str())) {
        ofFileDialogResult result = ofSystemLoadDialog("Choose a file to load", false);
        if (result.bSuccess) {
            std::string path = result.getPath();
            if (type.compare("MESH") == 0 ) {
                ofMesh& newMesh = this->_resourceManager.loadMesh(path);
                primaryRenderable->mesh = newMesh;

                primaryRenderable->isPrimitive = false;

                glm::vec3 minBounds(FLT_MAX, FLT_MAX, FLT_MAX);
                glm::vec3 maxBounds(-FLT_MAX, -FLT_MAX, -FLT_MAX);

                for (const auto& vertex : newMesh.getVertices()) {
                    minBounds.x = std::min(minBounds.x, vertex.x);
                    minBounds.y = std::min(minBounds.y, vertex.y);
                    minBounds.z = std::min(minBounds.z, vertex.z);
                    maxBounds.x = std::max(maxBounds.x, vertex.x);
                    maxBounds.y = std::max(maxBounds.y, vertex.y);
                    maxBounds.z = std::max(maxBounds.z, vertex.z);
                }

                glm::vec3 size = maxBounds - minBounds;

                this->_componentRegistry.removeComponent<Box>(entityId);
                this->_componentRegistry.removeComponent<Sphere>(entityId);
                this->_componentRegistry.removeComponent<Plane>(entityId);
                this->_componentRegistry.removeComponent<Triangle>(entityId);
                this->_componentRegistry.removeComponent<Circle>(entityId);
                this->_componentRegistry.removeComponent<Line>(entityId);
                this->_componentRegistry.removeComponent<Rectangle>(entityId);
                this->_componentRegistry.removeComponent<Point>(entityId);

                this->_componentRegistry.registerComponent(entityId, Box(size));
            } else {
                ofTexture& newTex = this->_resourceManager.loadTexture(path);
                primaryRenderable->material->texture = &newTex;
            }
        }
    }
}

void MaterialPanel::_generateProceduralTexture(Renderable* primaryRenderable)
{
    if (ImGui::Button("Generate Procedural"))
        ImGui::OpenPopup("ProceduralTexturePopup");

    if (ImGui::BeginPopup("ProceduralTexturePopup")) {
        ImGui::Text("Generate Procedural Texture");
        ImGui::Separator();

        static int selectedType = 0;
        const char* types[] = { "Perlin Noise", "Voronoi", "Checkerboard", "Gradient" };
        ImGui::Combo("Type", &selectedType, types, IM_ARRAYSIZE(types));

        static float color1[3] = {0.2f, 0.2f, 0.8f};
        static float color2[3] = {0.9f, 0.9f, 0.9f};
        ImGui::ColorEdit3("Color 1", color1);
        ImGui::ColorEdit3("Color 2", color2);

        static int resolution = 512;
        ImGui::SliderInt("Resolution", &resolution, 128, 2048);

        if (ImGui::Button("Generate", ImVec2(-1, 0))) {
            ProceduralTexture::Type type;
            switch (selectedType) {
                case 0: type = ProceduralTexture::Type::PERLIN_NOISE; break;
                case 1: type = ProceduralTexture::Type::VORONOI; break;
                case 2: type = ProceduralTexture::Type::CHECKERBOARD; break;
                case 3: type = ProceduralTexture::Type::GRADIENT; break;
                default: type = ProceduralTexture::Type::PERLIN_NOISE; break;
            }

            ofColor col1(color1[0] * 255, color1[1] * 255, color1[2] * 255);
            ofColor col2(color2[0] * 255, color2[1] * 255, color2[2] * 255);

            ofTexture generatedTex = this->_proceduralTextureGenerator.generate(type, resolution, resolution, col1, col2);

            std::string texName = "procedural_" + std::string(types[selectedType]) + "_" + std::to_string(resolution);
            ofTexture& storedTex = this->_resourceManager.storeTexture(texName, generatedTex);
            primaryRenderable->material->texture = &storedTex;

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void MaterialPanel::_renderVisibilityControl(const std::set<EntityID>& selectedEntities, Renderable* primaryRenderable)
{
    bool commonVisibility;
    bool visibilitySame = this->_checkAllEntitiesHaveSameVisibility(selectedEntities, commonVisibility);

    bool editVisibility = visibilitySame ? commonVisibility : primaryRenderable->visible;
    bool visibilityChanged = false;

    ImGui::BeginDisabled(!visibilitySame);
    visibilityChanged = ImGui::Checkbox("Visible", &editVisibility);
    ImGui::EndDisabled();

    if (!visibilitySame) visibilityChanged = false;

    if (visibilityChanged) {
        for (EntityID id : selectedEntities) {
            Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(id);
            if (renderable) renderable->visible = editVisibility;
        }
    }
}

void MaterialPanel::_renderShaderSection(EntityID primaryEntity, const std::set<EntityID>& selectedEntities, Renderable* primaryRenderable)
{
    if (primaryRenderable->material->shader) {
        ImGui::Text(" - Shader: Set");
        this->_loadShaders(selectedEntities, primaryRenderable);
        ImGui::SameLine();
        if (ImGui::Button("Clear Shader")) {
            for (EntityID id : selectedEntities) {
                Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(id);
                if (renderable && renderable->material) {
                    renderable->material->shader = nullptr;
                }
            }
        }
    } else {
        ImGui::Text(" - Shader: None");
        ImGui::SameLine();
        this->_loadShaders(selectedEntities, primaryRenderable);
    }
}

void MaterialPanel::_renderTextureSection(EntityID primaryEntity, Renderable* primaryRenderable)
{
    if (primaryRenderable->material->texture) {
        ofTexture* tex = primaryRenderable->material->texture;
        std::string texName = this->_resourceManager.getTexturePath(*tex);
        ImGui::Text(" - Texture: Set %s", texName.c_str());

        ImVec2 thumbSize = ImVec2(24, 24);
        GLuint texID = tex->getTextureData().textureID;
        ImGui::Image((ImTextureID)(uintptr_t)texID, thumbSize, ImVec2(0,1), ImVec2(1,0));

        ImGui::SameLine();
        this->_loadFile(primaryEntity, primaryRenderable, "TEX");

        ImGui::SameLine();
        if (ImGui::Button("Clear Texture"))
            primaryRenderable->material->texture = nullptr;
    } else {
        ImGui::Text(" - Texture: None");
        this->_loadFile(primaryEntity, primaryRenderable, "TEX");
        ImGui::SameLine();
        this->_generateProceduralTexture(primaryRenderable);
    }
}

void MaterialPanel::_renderMeshSection(EntityID primaryEntity, Renderable* primaryRenderable)
{
    if (primaryRenderable->mesh.getNumVertices() > 0) {
        ofMesh mesh = primaryRenderable->mesh;
        std::string meshName = this->_resourceManager.getMeshPath(mesh);
        ImGui::Text(" - Mesh: %s", meshName.c_str());
        ImGui::SameLine();
        this->_loadFile(primaryEntity, primaryRenderable, "MESH");
    } else {
        ImGui::Text(" - Mesh: None");
        ImGui::SameLine();
        this->_loadFile(primaryEntity, primaryRenderable, "MESH");
    }
}

void MaterialPanel::_renderLightingParameters(const std::set<EntityID>& selectedEntities, Renderable* primaryRenderable)
{
    if (!primaryRenderable->material->shader) return;

    ImGui::Text("Lighting Parameters:");

    if (ImGui::DragFloat3("Light Position", &primaryRenderable->material->lightPosition.x, 0.1f))
        this->_syncMaterialProperty(selectedEntities, &Material::lightPosition, primaryRenderable->material->lightPosition);

    if (ImGui::ColorEdit3("Light Color", &primaryRenderable->material->lightColor.x))
        this->_syncMaterialProperty(selectedEntities, &Material::lightColor, primaryRenderable->material->lightColor);

    if (ImGui::SliderFloat("Light Intensity", &primaryRenderable->material->lightIntensity, 0.0f, 5.0f))
        this->_syncMaterialProperty(selectedEntities, &Material::lightIntensity, primaryRenderable->material->lightIntensity);

    if (ImGui::ColorEdit3("Ambient Color", &primaryRenderable->material->ambientColor.x))
        this->_syncMaterialProperty(selectedEntities, &Material::ambientColor, primaryRenderable->material->ambientColor);

    if (ImGui::SliderFloat("Shininess", &primaryRenderable->material->shininess, 1.0f, 128.0f))
        this->_syncMaterialProperty(selectedEntities, &Material::shininess, primaryRenderable->material->shininess);

    ImGui::Separator();
    ImGui::Text("Reflection Parameters:");

    if (ImGui::SliderFloat("Reflectivity", &primaryRenderable->material->reflectivity, 0.0f, 1.0f))
        this->_syncMaterialProperty(selectedEntities, &Material::reflectivity, primaryRenderable->material->reflectivity);

    if (ImGui::ColorEdit3("Reflection Tint", &primaryRenderable->material->reflectionTint.x))
        this->_syncMaterialProperty(selectedEntities, &Material::reflectionTint, primaryRenderable->material->reflectionTint);

    ImGui::Separator();
}

void MaterialPanel::_renderReliefMappingSection(EntityID primaryEntity, const std::set<EntityID>& selectedEntities, Renderable* primaryRenderable)
{
    if (ImGui::CollapsingHeader("Relief Mapping", ImGuiTreeNodeFlags_DefaultOpen)) {

        ImGui::Text("Normal Mapping:");
        ImGui::Indent();

        if (primaryRenderable->material->normalMap) {
            ofTexture* normalTex = primaryRenderable->material->normalMap;
            std::string normalTexName = this->_resourceManager.getTexturePath(*normalTex);
            ImGui::Text(" - Normal Map: %s", normalTexName.c_str());

            ImVec2 thumbSize = ImVec2(64, 64);
            GLuint texID = normalTex->getTextureData().textureID;
            ImGui::Image((ImTextureID)(uintptr_t)texID, thumbSize, ImVec2(0,1), ImVec2(1,0));

            if (ImGui::Button("Clear Normal Map")) {
                for (EntityID id : selectedEntities) {
                    Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(id);
                    if (renderable && renderable->material) {
                        renderable->material->normalMap = nullptr;
                    }
                }
            }
        } else {
            ImGui::Text(" - Normal Map: None");
        }

        if (ImGui::Button("Load Normal Map")) {
            ImGui::OpenPopup("LoadNormalMapPopup");
        }

        if (ImGui::BeginPopup("LoadNormalMapPopup")) {
            std::filesystem::path normalMapDir = std::filesystem::path(ofToDataPath("normalmaps"));
            std::vector<std::string> normalMapFiles;

            if (std::filesystem::exists(normalMapDir) && std::filesystem::is_directory(normalMapDir)) {
                for (const auto& entry : std::filesystem::directory_iterator(normalMapDir)) {
                    if (entry.is_regular_file()) {
                        std::string ext = entry.path().extension().string();
                        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") {
                            normalMapFiles.push_back(entry.path().filename().string());
                        }
                    }
                }
            }

            if (normalMapFiles.empty()) {
                ImGui::TextDisabled("No normal maps found in data/normalmaps");
            } else {
                for (const std::string& filename : normalMapFiles) {
                    if (ImGui::Selectable(filename.c_str())) {
                        std::string path = (normalMapDir / filename).string();
                        ofTexture& loadedNormalMap = this->_resourceManager.loadTexture(path);

                        for (EntityID id : selectedEntities) {
                            Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(id);
                            if (renderable && renderable->material) {
                                renderable->material->normalMap = &loadedNormalMap;
                            }
                        }

                        ImGui::CloseCurrentPopup();
                    }
                }
            }

            ImGui::EndPopup();
        }

        if (primaryRenderable->material->normalMap) {
            if (ImGui::SliderFloat("Normal Strength", &primaryRenderable->material->normalStrength, 0.0f, 2.0f)) {
                this->_syncMaterialProperty(selectedEntities, &Material::normalStrength, primaryRenderable->material->normalStrength);
            }
        }

        ImGui::Unindent();
        ImGui::Separator();

        ImGui::Text("Displacement Mapping:");
        ImGui::Indent();

        if (primaryRenderable->material->heightMap) {
            ofTexture* heightTex = primaryRenderable->material->heightMap;
            std::string heightTexName = this->_resourceManager.getTexturePath(*heightTex);
            ImGui::Text(" - Height Map: %s", heightTexName.c_str());

            ImVec2 thumbSize = ImVec2(64, 64);
            GLuint texID = heightTex->getTextureData().textureID;
            ImGui::Image((ImTextureID)(uintptr_t)texID, thumbSize, ImVec2(0,1), ImVec2(1,0));

            if (ImGui::Button("Clear Height Map")) {
                for (EntityID id : selectedEntities) {
                    Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(id);
                    if (renderable && renderable->material) {
                        renderable->material->heightMap = nullptr;

                        if (renderable->isPrimitive) {
                            this->_primitiveSystem.regenerateMesh(id);
                        }

                        this->_componentRegistry.removeComponent<DisplacementMap>(id);
                    }
                }
            }
        } else {
            ImGui::Text(" - Height Map: None");
        }

        if (ImGui::Button("Load Height Map")) {
            ImGui::OpenPopup("LoadHeightMapPopup");
        }

        if (ImGui::BeginPopup("LoadHeightMapPopup")) {
            std::filesystem::path heightMapDir = std::filesystem::path(ofToDataPath("heightmaps"));
            std::vector<std::string> heightMapFiles;

            if (std::filesystem::exists(heightMapDir) && std::filesystem::is_directory(heightMapDir)) {
                for (const auto& entry : std::filesystem::directory_iterator(heightMapDir)) {
                    if (entry.is_regular_file()) {
                        std::string ext = entry.path().extension().string();
                        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") {
                            heightMapFiles.push_back(entry.path().filename().string());
                        }
                    }
                }
            }

            if (heightMapFiles.empty()) {
                ImGui::TextDisabled("No height maps found in data/heightmaps");
            } else {
                for (const std::string& filename : heightMapFiles) {
                    if (ImGui::Selectable(filename.c_str())) {
                        std::string path = (heightMapDir / filename).string();
                        ofTexture& loadedHeightMap = this->_resourceManager.loadTexture(path);

                        for (EntityID id : selectedEntities) {
                            Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(id);
                            if (renderable && renderable->material) {
                                renderable->material->heightMap = &loadedHeightMap;
                            }
                        }

                        ImGui::CloseCurrentPopup();
                    }
                }
            }

            ImGui::EndPopup();
        }

        if (primaryRenderable->material->heightMap) {
            DisplacementMap* displacement = this->_componentRegistry.getComponent<DisplacementMap>(primaryEntity);

            if (!displacement) {
                if (ImGui::Button("Enable Displacement")) {
                    this->_componentRegistry.registerComponent(primaryEntity, DisplacementMap(0.5f, 3));
                }
            } else {
                if (ImGui::SliderFloat("Displacement Strength", &displacement->strength, 0.0f, 2.0f)) {
                    displacement->needsRegeneration = true;
                }

                if (ImGui::SliderInt("Subdivision Level", &displacement->subdivisionLevel, 0, 5)) {
                    displacement->needsRegeneration = true;
                }

                if (ImGui::Button("Apply Displacement")) {
                    this->_primitiveSystem.applyDisplacement(primaryEntity);
                }

                ImGui::SameLine();
                if (ImGui::Button("Remove Displacement Component")) {
                    this->_componentRegistry.removeComponent<DisplacementMap>(primaryEntity);
                }
            }
        }

        ImGui::Unindent();
    }
}
