#include "Systems/CameraSystem/CameraSystem.hpp"

CameraSystem::CameraSystem(ComponentRegistry& registry, TransformSystem& transformSystem)
    : _componentRegistry(registry), _transformSystem(transformSystem)
{
}

void CameraSystem::pan(EntityID camEntity, float horizontal, float vertical, float depth)
{
    Camera *cam = this->_componentRegistry.getComponent<Camera>(camEntity);
    if (!cam) return;

    cam->focusMode = false;
    this->_transformSystem.setPosition(camEntity, {horizontal * cam->panSensitivity, vertical * cam->panSensitivity, depth * cam->panSensitivity});
}

void CameraSystem::zoom(EntityID camEntity, float amount)
{
    float deltaTime = ofGetLastFrameTime();
    Camera *cam = this->_componentRegistry.getComponent<Camera>(camEntity);
    Transform *transform = this->_componentRegistry.getComponent<Transform>(camEntity);

    float newFov = cam->fov - deltaTime * amount * cam->zoomSensitivity;
    newFov = glm::clamp(newFov, cam->minFov, cam->maxFov);
    cam->fov = newFov;

    if (cam->focusMode) {
        cam->viewMatrix = glm::lookAt(
            transform->position,
            cam->target,
            cam->up
        );
    }
}

void CameraSystem::rotate(EntityID camEntity, float horizontal, float vertical)
{
    Camera* cam = this->_componentRegistry.getComponent<Camera>(camEntity);
    Transform* transform = this->_componentRegistry.getComponent<Transform>(camEntity);
    if (!cam || !transform) return;

    glm::vec3 pivot = cam->focusMode ? cam->target : (transform->position + cam->forward);

    this->_transformSystem.setRotation(camEntity, {horizontal * cam->rotateSensitivity, vertical * cam->rotateSensitivity}, &pivot);

    glm::vec3 newForward = glm::normalize(pivot - transform->position);
    cam->forward = newForward;
    cam->up = glm::vec3(0, 1, 0);
}

void CameraSystem::update(std::vector<EntityID> cameraEntities, int viewportWidth, int viewportHeight)
{
    for (EntityID id : cameraEntities)
    {
        Camera *cam = this->_componentRegistry.getComponent<Camera>(id);
        Transform *transform = this->_componentRegistry.getComponent<Transform>(id);
        if (cam && transform)
        {
            cam->aspectRatio = static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight);
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

glm::vec3 CameraSystem::_getCameraForward(EntityID id) const
{
    Transform *transform = this->_componentRegistry.getComponent<Transform>(id);
    Camera *cam = this->_componentRegistry.getComponent<Camera>(id);
    if (!transform || !cam)
        return glm::vec3(0, 0, -1);
    return glm::normalize(cam->target - transform->position);
}

glm::vec3 CameraSystem::_getCameraRight(EntityID id) const
{
    glm::vec3 forward = this->_getCameraForward(id);
    Camera *cam = this->_componentRegistry.getComponent<Camera>(id);
    if (!cam)
        return glm::vec3(1, 0, 0);
    return glm::normalize(glm::cross(forward, cam->up));
}

ComponentRegistry &CameraSystem::getRegistry() const
{
    return this->_componentRegistry;
}
