#pragma once

#include "Components/Renderable.hpp"
#include "Components/Transform.hpp"
#include "Components/Camera.hpp"
#include "Core/EntityManager/EntityManager.hpp"
#include "Core/ComponentRegistry/ComponentRegistry.hpp"
#include "Systems/CameraSystem/CameraSystem.hpp"

class RenderSystem {
    public:
        RenderSystem(ComponentRegistry& registry, EntityManager& entityMgr, CameraSystem& camSystem);
        ~RenderSystem() = default;

        void render();

    private:
        ComponentRegistry& _registry;
        EntityManager& _entityManager;
        CameraSystem& _cameraSystem;

        void setupRenderState();
        ofCamera buildCameraFromComponents(Camera& camera, const Transform& transform);
        void renderEntities(ofCamera& cam);
        void drawMesh(const ofMesh& mesh, const glm::mat4& transform, const ofColor& color, Material *material = nullptr);
};
