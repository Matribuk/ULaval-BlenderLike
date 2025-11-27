#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <limits>

#include "Components/Camera.hpp"
#include "Components/Renderable.hpp"
#include "Components/Transform.hpp"
#include "Components/BoundingBoxVisualization.hpp"
#include "Components/CustomBounds.hpp"
#include "Components/LightSource.hpp"

#include "Manager/CameraManager.hpp"

#include "Core/ComponentRegistry.hpp"
#include "Core/EntityManager.hpp"
#include "Core/Cubemap.hpp"
#include "Systems/SelectionSystem.hpp"

class SelectionSystem;

class RenderSystem {
    public:
        RenderSystem(ComponentRegistry& registry, EntityManager& entityMgr);
        ~RenderSystem() = default;

        void render();
        void loadCubemap(const std::string& folderPath);
        void setup(CameraManager& cameraManager, SelectionSystem& selectionSystem);

    private:
        ComponentRegistry& _registry;
        EntityManager& _entityManager;
        CameraManager* _cameraManager = nullptr;
        SelectionSystem* _selectionSystem = nullptr;

        void _setupRenderState();
        void _renderEntities();
        void _drawMesh(const ofMesh& mesh, const glm::mat4& transform, const ofColor& color, Material *material = nullptr, bool isSelected = false);
        void _drawMeshSinglePass(const ofMesh& mesh, const glm::mat4& transform, const ofColor& color, Material* material, ofShader* shader, bool isIllumination);
        void _drawMeshMultiPass(const ofMesh& mesh, const glm::mat4& transform, const ofColor& color, Material* material);
        void _drawBoundingBox(EntityID entityId, const Transform& transform, const BoundingBoxVisualization& bboxVis);
        void _collectLights(std::vector<LightSource>& lights);
        void _setLightUniforms(ofShader* shader, const std::vector<LightSource>& lights);
        void _drawLightDirectionIndicator(const LightSource& light, const glm::mat4& transform);

        ofShader _skyCubeShader;
        ofVboMesh _skyQuad;
        Cubemap _skyboxCubemap;
        bool _skyInitialized = false;

        ofTexture _whiteTexture;

        float _boundingBoxSize = 2.0f;

        void _initSkybox();
        void _renderSkyboxCubemap();
        void _initWhiteTexture();
};
