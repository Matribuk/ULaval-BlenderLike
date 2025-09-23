#include "CameraSystem.hpp"

CameraSystem::CameraSystem(ComponentRegistry& registry, EntityManager& entityMgr)
    : _registry(registry), _entityManager(entityMgr) {}

void CameraSystem::update() {
    for (EntityID id : _entityManager.getAllEntities()) {
        Camera* cam = _registry.getComponent<Camera>(id);
        Transform* transform = _registry.getComponent<Transform>(id);

        if (cam && transform) {
            if (cam->focusMode)
                cam->viewMatrix = glm::lookAt(transform->position, cam->target, cam->up);
            else
                cam->viewMatrix = glm::lookAt(transform->position, transform->position + cam->forward, cam->up);

            cam->projMatrix = glm::perspective(
                glm::radians(cam->fov),
                cam->aspectRatio,
                cam->nearClip,
                cam->farClip
            );
        }
    }
}
