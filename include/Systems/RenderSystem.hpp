#pragma once

#include "Components/Camera.hpp"
#include "Components/Renderable.hpp"
#include "Components/Transform.hpp"

#include "Manager/CameraManager.hpp"

#include "Core/ComponentRegistry.hpp"
#include "Core/EntityManager.hpp"

class RenderSystem {
    public:
        RenderSystem(ComponentRegistry& registry, EntityManager& entityMgr, CameraManager& cameraManager);
        ~RenderSystem() = default;

        void render();
        void setSkyColors(const glm::vec3& top, const glm::vec3& bottom);

    private:
        ComponentRegistry& _registry;
        EntityManager& _entityManager;
        CameraManager& _cameraManager;

        void _setupRenderState();
        ofCamera _buildCameraFromComponents(Camera& camera, const Transform& transform);
        void _renderEntities();
        void _drawMesh(const ofMesh& mesh, const glm::mat4& transform, const ofColor& color, Material *material = nullptr);

        ofShader _skyShader;
        ofVboMesh _skyQuad;
        bool _skyInitialized = false;
        glm::vec3 _skyTopColor{0.529f, 0.808f, 0.922f};
        glm::vec3 _skyBottomColor{0.9f, 0.95f, 1.0f};

        void _initSkybox();
        void _renderSkybox();
};
