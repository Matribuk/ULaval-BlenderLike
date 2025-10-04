#include "CameraSystem.hpp"

CameraSystem::CameraSystem(ComponentRegistry& registry, EntityManager& entityMgr)
    : _componentRegistry(registry), _entityManager(entityMgr)
{
}

Camera* CameraSystem::getActiveCamera() const {
    if (_cameraEntities.empty()) return nullptr;
    return _componentRegistry.getComponent<Camera>(_cameraEntities[_activeCameraIndex]);
}

EntityID CameraSystem::getActiveCameraId() const {
    if (_cameraEntities.empty()) return 0;
    return _cameraEntities[_activeCameraIndex];
}

void CameraSystem::addCamera(glm::vec3 pos) {
    EntityID cameraID = _entityManager.createEntity().getId();
    Transform transform(pos);
    if (std::find(_cameraEntities.begin(), _cameraEntities.end(), cameraID) == _cameraEntities.end()) {
        _componentRegistry.registerComponent<Transform>(cameraID, transform);
        _componentRegistry.registerComponent<Camera>(cameraID, Camera());
        _cameraEntities.push_back(cameraID);
        if (_activeCamera == INVALID_ENTITY) setActiveCamera(cameraID);
    }
}

void CameraSystem::setActiveCamera(EntityID id) {
    for (size_t i = 0; i < _cameraEntities.size(); ++i) {
        if (_cameraEntities[i] == id) {
            _activeCameraIndex = static_cast<int>(i);
            _activeCamera = id;
            return;
        }
    }
}

void CameraSystem::switchCamera() {
    if (_cameraEntities.empty() || _activeCamera == INVALID_ENTITY) return;
    _activeCameraIndex = (_activeCameraIndex + 1) % static_cast<int>(_cameraEntities.size());
    _activeCamera = _cameraEntities[_activeCameraIndex];
}

void CameraSystem::panKeyboard(float horizontal, float vertical) {
    if (_cameraEntities.empty() || _activeCamera == INVALID_ENTITY)
        return;
    Camera* cam = getActiveCamera();
    Transform* t = _componentRegistry.getComponent<Transform>(_activeCamera);
    if (!cam || !t) return;

    cam->focusMode = false;

    glm::vec3 right = _getCameraRight(_activeCamera);
    glm::vec3 up = cam->up;

    t->position += right * horizontal * cam->panSensitivity + up * vertical * cam->panSensitivity;
}

void CameraSystem::orbitKeyboard(float horizontal, float vertical) {
    if (_cameraEntities.empty() || _activeCamera == INVALID_ENTITY) return;
    Camera* cam = _componentRegistry.getComponent<Camera>(_activeCamera);
    Transform* t = _componentRegistry.getComponent<Transform>(_activeCamera);
    if (!cam || !t) return;

    float angleY = horizontal * cam->orbitSensitivity;
    float angleX = vertical * cam->orbitSensitivity;

    glm::vec3 pivot = cam->focusMode ? cam->target : t->position + cam->forward * 5.0f;

    glm::vec3 dir = t->position - pivot;

    glm::vec3 forward = glm::normalize(-dir);
    glm::vec3 right = glm::normalize(glm::cross(forward, cam->up));
    glm::vec3 up = glm::normalize(glm::cross(right, forward));

    glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), angleY, up);
    dir = glm::vec3(rotY * glm::vec4(dir, 0.0f));

    glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), angleX, right);
    dir = glm::vec3(rotX * glm::vec4(dir, 0.0f));

    t->position = pivot + dir;

    cam->forward = glm::normalize(pivot - t->position);
    cam->up = up;
}

void CameraSystem::zoom(float amount) {
    if (_cameraEntities.empty() || _activeCamera == INVALID_ENTITY) return;
    Camera* cam = getActiveCamera();
    Transform* t = _componentRegistry.getComponent<Transform>(_activeCamera);
    if (!cam || !t) return;

    glm::vec3 forward = _getCameraForward(_activeCamera);
    float distance = glm::length(cam->target - t->position);
    distance = glm::clamp(distance - amount * cam->zoomSensitivity, cam->minDistance, cam->maxDistance);
    t->position = cam->target - forward * distance;
}

void CameraSystem::focusTarget() {
    if (_cameraEntities.empty() || _activeCamera == INVALID_ENTITY) return;
    Camera* cam = getActiveCamera();
    Transform* t = _componentRegistry.getComponent<Transform>(_activeCamera);
    if (!cam || !t) return;

    cam->focusMode = true;

    glm::vec3 dir = glm::normalize(t->position - cam->target);
    float distance = glm::length(t->position - cam->target);

    if (distance < cam->minDistance || distance > cam->maxDistance) {
        distance = glm::clamp(distance, cam->minDistance, cam->maxDistance);
    }

    t->position = cam->target - cam->forward * distance;
}

void CameraSystem::update() {
    if (_cameraEntities.empty() || _activeCamera == INVALID_ENTITY) return;
    for (EntityID id : _cameraEntities) {
        Camera* cam = _componentRegistry.getComponent<Camera>(id);
        Transform* t = _componentRegistry.getComponent<Transform>(id);
        if (!cam || !t) continue;

        if (cam->focusMode)
            cam->viewMatrix = glm::lookAt(t->position, cam->target, cam->up);
        else
            cam->viewMatrix = glm::lookAt(t->position, t->position + cam->forward, cam->up);

        cam->projMatrix = glm::perspective(glm::radians(cam->fov),
                                           cam->aspectRatio,
                                           cam->nearClip,
                                           cam->farClip);
    }
}

glm::vec3 CameraSystem::_getCameraForward(EntityID id) const {
    Transform* t = _componentRegistry.getComponent<Transform>(id);
    Camera* cam = _componentRegistry.getComponent<Camera>(id);
    if (!t || !cam) return glm::vec3(0, 0, -1);
    return glm::normalize(cam->target - t->position);
}

glm::vec3 CameraSystem::_getCameraRight(EntityID id) const {
    glm::vec3 forward = _getCameraForward(id);
    Camera* cam = _componentRegistry.getComponent<Camera>(id);
    if (!cam) return glm::vec3(1, 0, 0);
    return glm::normalize(glm::cross(forward, cam->up));
}

EntityID CameraSystem::getCameraAtIndex(int index) const {
    if (index < 0 || index >= static_cast<int>(_cameraEntities.size())) return INVALID_ENTITY;
    return _cameraEntities[index];
}
