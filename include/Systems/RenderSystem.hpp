#pragma once

#include "Components/Camera.hpp"
#include "Components/Renderable.hpp"
#include "Components/Transform.hpp"

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
        ofCamera _buildCameraFromComponents(Camera& camera, const Transform& transform);
        void _renderEntities();
        void _drawMesh(const ofMesh& mesh, const glm::mat4& transform, const ofColor& color, Material *material = nullptr, bool isSelected = false);

        // Cubemap skybox
        ofShader _skyCubeShader;
        ofVboMesh _skyQuad;
        Cubemap _skyboxCubemap;
        bool _skyInitialized = false;

        float _boundingBoxSize = 2.0f;

        void _initSkybox();
        void _renderSkyboxCubemap();
};
