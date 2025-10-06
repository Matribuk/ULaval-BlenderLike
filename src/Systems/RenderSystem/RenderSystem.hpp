#pragma once

#include "Components/Renderable.hpp"
#include "Components/Transform.hpp"
#include "Components/Camera.hpp"
#include "Core/EntityManager/EntityManager.hpp"
#include "Core/ComponentRegistry/ComponentRegistry.hpp"
#include "Manager/CameraManager/CameraManager.hpp"

class RenderSystem {
    public:
        RenderSystem(ComponentRegistry& registry, EntityManager& entityMgr, CameraManager& cameraManager);
        ~RenderSystem() = default;

        void render();
        void setActiveCamera(EntityID cameraEntity);

        Camera *getActiveCameraObject() const;
        EntityID getActiveCameraId() const;

    private:
        ComponentRegistry& _registry;
        EntityManager& _entityManager;
        CameraManager& _cameraManager;

        void setupRenderState();
        ofCamera buildCameraFromComponents(Camera& camera, const Transform& transform);
        void renderEntities();
        void drawMesh(const ofMesh& mesh, const glm::mat4& transform, const ofColor& color, Material *material = nullptr);
};
