#include "Systems/CameraSystem/CameraSystem.hpp"

CameraSystem::CameraSystem(ComponentRegistry &registry)
    : _componentRegistry(registry)
{
}

void CameraSystem::_panKeyboard(EntityID camEntity, float horizontal, float vertical, float depth)
{
    Camera *cam = this->_componentRegistry.getComponent<Camera>(camEntity);
    Transform *transform = this->_componentRegistry.getComponent<Transform>(camEntity);
    if (!cam || !transform) {
        std::cerr << "Camera or Transform component is missing!" << std::endl;
        return;
    }

    cam->focusMode = false;

    glm::vec3 right = this->_getCameraRight(camEntity);
    glm::vec3 up = cam->up;
    glm::vec3 forward = glm::normalize(cam->forward);
    transform->position += right * horizontal * cam->panSensitivity
                     + up * vertical * cam->panSensitivity
                     + forward * depth * cam->panSensitivity;

}

void CameraSystem::_orbitKeyboard(EntityID camEntity, float horizontal, float vertical)
{
    Camera* cam = this->_componentRegistry.getComponent<Camera>(camEntity);
    Transform* transform = this->_componentRegistry.getComponent<Transform>(camEntity);
    if (!cam || !transform) return;

    glm::vec3 pivot = cam->focusMode ? cam->target : (transform->position + cam->forward);
    glm::vec3 offset = transform->position - pivot;
    float radius = glm::length(offset);

    float yaw   = atan2(offset.x, offset.z);
    float pitch = asin(offset.y / radius);

    yaw   += horizontal * cam->orbitSensitivity;
    pitch += vertical * cam->orbitSensitivity;

    float cosPitch = cos(pitch);
    glm::vec3 newOffset;
    newOffset.x = radius * cosPitch * sin(yaw);
    newOffset.y = radius * sin(pitch);
    newOffset.z = radius * cosPitch * cos(yaw);

    transform->position = pivot + newOffset;
    cam->forward = glm::normalize(pivot - transform->position);
    cam->up = glm::vec3(0, 1, 0);
}

void CameraSystem::_zoom(EntityID camEntity, float amount)
{
    Camera *cam = this->_componentRegistry.getComponent<Camera>(camEntity);
    Transform *transform = this->_componentRegistry.getComponent<Transform>(camEntity);
    if (!cam || !transform) {
        std::cerr << "Camera or Transform component is missing!" << std::endl;
        return;
    }

    glm::vec3 forward = glm::normalize(cam->forward);
    if (cam->focusMode) {
        float distance = glm::length(cam->target - transform->position);
        distance = glm::clamp(distance - amount * cam->zoomSensitivity, cam->minDistance, cam->maxDistance);
        transform->position = cam->target - forward * distance;
    } else {
        transform->position += forward * amount * cam->zoomSensitivity;
    }
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

void CameraSystem::handleCameraMovement(EntityID camEntity)
{
    float deltaTime = ofGetLastFrameTime();

    if (this->_zoomInput != 0.0f)
        this->_zoom(camEntity, this->_zoomInput * deltaTime);
    if (this->_panInput.x != 0.0f || this->_panInput.y != 0.0f || this->_panInput.z)
        this->_panKeyboard(camEntity, this->_panInput.x * deltaTime, this->_panInput.y * deltaTime, this->_panInput.z * deltaTime);
    if (this->_orbitInput.x != 0.0f || this->_orbitInput.y != 0.0f)
        this->_orbitKeyboard(camEntity, this->_orbitInput.x * deltaTime, this->_orbitInput.y * deltaTime);

    this->_zoomInput = 0.0f;
    this->_panInput = glm::vec3(0.0f);
    this->_orbitInput = glm::vec2(0.0f);
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

void CameraSystem::updateZoomInput(float zoom)
{
    this->_zoomInput += zoom;
}

void CameraSystem::updatePanInput(float x, float y, float depth)
{
    this->_panInput += glm::vec3(x, y, depth);
}

void CameraSystem::updateOrbitInput(float x, float y)
{
    this->_orbitInput += glm::vec2(x, y);
}
