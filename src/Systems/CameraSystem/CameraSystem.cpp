#include "CameraSystem.hpp"

CameraSystem::CameraSystem(ComponentRegistry &registry)
    : _componentRegistry(registry)
{
}

void CameraSystem::_panKeyboard(EntityID camEntity, float horizontal, float vertical)
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

    transform->position += right * horizontal * cam->panSensitivity + up * vertical * cam->panSensitivity;
}

void CameraSystem::_orbitKeyboard(EntityID camEntity, float horizontal, float vertical)
{
    Camera *cam = this->_componentRegistry.getComponent<Camera>(camEntity);
    Transform *transform = this->_componentRegistry.getComponent<Transform>(camEntity);
    if (!cam || !transform) return;

    float angleY = horizontal * cam->orbitSensitivity;
    float angleX = vertical * cam->orbitSensitivity;

    glm::vec3 pivot = cam->focusMode ? cam->target : (transform->position + cam->forward * 5.0f);
    glm::vec3 dir = transform->position - pivot;
    glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), angleY, glm::vec3(0, 1, 0));
    dir = glm::vec3(rotY * glm::vec4(dir, 0.0f));

    glm::vec3 forward = glm::normalize(-dir);
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
    glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), angleX, right);
    dir = glm::vec3(rotX * glm::vec4(dir, 0.0f));

    transform->position = pivot + dir;
    cam->forward = glm::normalize(pivot - transform->position);
    cam->up = glm::normalize(glm::cross(right, cam->forward));
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
        _zoom(camEntity, this->_zoomInput * deltaTime);
    if (this->_panInput.x != 0.0f || this->_panInput.y != 0.0f)
        _panKeyboard(camEntity, this->_panInput.x * deltaTime, this->_panInput.y * deltaTime);
    if (this->_orbitInput.x != 0.0f || this->_orbitInput.y != 0.0f)
        _orbitKeyboard(camEntity, this->_orbitInput.x * deltaTime, this->_orbitInput.y * deltaTime);

    this->_zoomInput = 0.0f;
    this->_panInput = glm::vec2(0.0f);
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
    _zoomInput += zoom;
}

void CameraSystem::updatePanInput(float x, float y)
{
    _panInput += glm::vec2(x, y);
}

void CameraSystem::updateOrbitInput(float x, float y)
{
    _orbitInput += glm::vec2(x, y);
}
