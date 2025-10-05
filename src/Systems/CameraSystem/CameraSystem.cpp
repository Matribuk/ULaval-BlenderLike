#include "CameraSystem.hpp"

CameraSystem::CameraSystem(ComponentRegistry& registry, EntityManager& entityMgr)
    : _componentRegistry(registry), _entityManager(entityMgr)
{
}

Camera* CameraSystem::getActiveCamera() const {
    if (this->_cameraEntities.empty()) return nullptr;
    return this->_componentRegistry.getComponent<Camera>(this->_cameraEntities[this->_activeCameraIndex]);
}

EntityID CameraSystem::getActiveCameraId() const {
    if (this->_cameraEntities.empty()) return 0;
    return this->_cameraEntities[this->_activeCameraIndex];
}

void CameraSystem::addCamera(glm::vec3 pos) {
    EntityID cameraID = this->_entityManager.createEntity().getId();
    Transform transform(pos);
    if (std::find(this->_cameraEntities.begin(), this->_cameraEntities.end(), cameraID) == this->_cameraEntities.end()) {
        this->_componentRegistry.registerComponent<Transform>(cameraID, transform);
        this->_componentRegistry.registerComponent<Camera>(cameraID, Camera());
        this->_cameraEntities.push_back(cameraID);
        if (this->_activeCamera == INVALID_ENTITY) setActiveCamera(cameraID);
    }
}

void CameraSystem::setActiveCamera(EntityID id) {
    for (size_t i = 0; i < this->_cameraEntities.size(); ++i) {
        if (this->_cameraEntities[i] == id) {
            this->_activeCameraIndex = static_cast<int>(i);
            this->_activeCamera = id;
            return;
        }
    }
}

void CameraSystem::switchCamera() {
    if (this->_cameraEntities.empty() || this->_activeCamera == INVALID_ENTITY) return;
    this->_activeCameraIndex = (this->_activeCameraIndex + 1) % static_cast<int>(this->_cameraEntities.size());
    this->_activeCamera = this->_cameraEntities[this->_activeCameraIndex];
}

void CameraSystem::panKeyboard(float horizontal, float vertical) {
    if (this->_cameraEntities.empty() || this->_activeCamera == INVALID_ENTITY)
        return;
    Camera* cam = getActiveCamera();
    Transform* t = this->_componentRegistry.getComponent<Transform>(this->_activeCamera);
    if (!cam || !t) return;

    cam->focusMode = false;

    glm::vec3 right = this->_getCameraRight(this->_activeCamera);
    glm::vec3 up = cam->up;

    t->position += right * horizontal * cam->panSensitivity + up * vertical * cam->panSensitivity;
}

void CameraSystem::orbitKeyboard(float horizontal, float vertical) {
    if (this->_cameraEntities.empty() || this->_activeCamera == INVALID_ENTITY) return;
    Camera* cam = this->_componentRegistry.getComponent<Camera>(this->_activeCamera);
    Transform* t = this->_componentRegistry.getComponent<Transform>(this->_activeCamera);
    if (!cam || !t) return;

    float angleY = horizontal * cam->orbitSensitivity;
    float angleX = vertical * cam->orbitSensitivity;

    glm::vec3 pivot = (cam->focusMode) ? (cam->target) : (t->position + cam->forward * 5.0f);

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
    if (this->_cameraEntities.empty() || this->_activeCamera == INVALID_ENTITY) return;
    Camera* cam = getActiveCamera();
    Transform* t = this->_componentRegistry.getComponent<Transform>(this->_activeCamera);
    if (!cam || !t) return;

    glm::vec3 forward = this->_getCameraForward(this->_activeCamera);
    float distance = glm::length(cam->target - t->position);
    distance = glm::clamp(distance - amount * cam->zoomSensitivity, cam->minDistance, cam->maxDistance);
    t->position = cam->target - forward * distance;
}

void CameraSystem::focusTarget() {
    if (this->_cameraEntities.empty() || this->_activeCamera == INVALID_ENTITY) return;
    Camera* cam = getActiveCamera();
    Transform* t = this->_componentRegistry.getComponent<Transform>(this->_activeCamera);
    if (!cam || !t) return;

    cam->focusMode = true;

    float distance = glm::length(t->position - cam->target);

    if (distance < cam->minDistance || distance > cam->maxDistance) {
        distance = glm::clamp(distance, cam->minDistance, cam->maxDistance);
    }

    t->position = cam->target - cam->forward * distance;
}

void CameraSystem::update(int viewportWidth, int viewportHeight) {
    float deltaTime = ofGetLastFrameTime();
    if (this->_cameraEntities.empty() || this->_activeCamera == INVALID_ENTITY) return;
    float newAspect = static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight);

    if (this->_zoomInput != 0.0f)
        zoom(this->_zoomInput * deltaTime);

    if (this->_panInput.x != 0.0f || this->_panInput.y != 0.0f)
        panKeyboard(this->_panInput.x * deltaTime, this->_panInput.y * deltaTime);

    if (this->_orbitInput.x != 0.0f || this->_orbitInput.y != 0.0f)
        orbitKeyboard(this->_orbitInput.x * deltaTime, this->_orbitInput.y * deltaTime);

    this->_zoomInput = 0.0f;
    this->_panInput = glm::vec2(0.0f);
    this->_orbitInput = glm::vec2(0.0f);

    for (EntityID id : this->_cameraEntities) {
        Camera* cam = this->_componentRegistry.getComponent<Camera>(id);
        Transform* transform = this->_componentRegistry.getComponent<Transform>(id);
        if (cam && transform) {
            cam->aspectRatio = newAspect;
            if (cam->focusMode)
                cam->viewMatrix = glm::lookAt(transform->position, cam->target, cam->up);
            else
                cam->viewMatrix = glm::lookAt(transform->position, transform->position + cam->forward, cam->up);

            cam->projMatrix = glm::perspective(glm::radians(cam->fov),
                                               cam->aspectRatio,
                                               cam->nearClip,
                                               cam->farClip);
        }
    }
}

glm::vec3 CameraSystem::_getCameraForward(EntityID id) const {
    Transform* t = this->_componentRegistry.getComponent<Transform>(id);
    Camera* cam = this->_componentRegistry.getComponent<Camera>(id);
    if (!t || !cam) return glm::vec3(0, 0, -1);
    return glm::normalize(cam->target - t->position);
}

glm::vec3 CameraSystem::_getCameraRight(EntityID id) const {
    glm::vec3 forward = this->_getCameraForward(id);
    Camera* cam = this->_componentRegistry.getComponent<Camera>(id);
    if (!cam) return glm::vec3(1, 0, 0);
    return glm::normalize(glm::cross(forward, cam->up));
}

EntityID CameraSystem::getCameraAtIndex(int index) const {
    if (index < 0 || index >= static_cast<int>(this->_cameraEntities.size())) return INVALID_ENTITY;
    return this->_cameraEntities[index];
}

EntityManager& CameraSystem::getEntityManager(){
    return this->_entityManager;
}

ComponentRegistry& CameraSystem::getRegistry()
{
    return this->_componentRegistry;
}
