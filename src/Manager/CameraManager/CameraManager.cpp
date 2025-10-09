#include "Manager/CameraManager/CameraManager.hpp"

CameraManager::CameraManager(ComponentRegistry &componentRegistry, EntityManager &entityManager, CameraSystem &cameraSystem)
    : _componentRegistry(componentRegistry), _entityManager(entityManager), _cameraSystem(cameraSystem) {}

EntityID CameraManager::addCamera(glm::vec3 pos)
{
    EntityID cameraID = this->_entityManager.createEntity().getId();
    Transform transform(pos);
    if (std::find(this->_cameraEntities.begin(), this->_cameraEntities.end(), cameraID) == this->_cameraEntities.end())
    {
        this->_componentRegistry.registerComponent<Transform>(cameraID, transform);
        this->_componentRegistry.registerComponent<Camera>(cameraID, Camera());
        this->_cameraEntities.push_back(cameraID);
        if (this->_activeCamera == INVALID_ENTITY)
            setActiveCamera(cameraID);
    }

    return cameraID;
}

void CameraManager::removeCamera(EntityID cameraId)
{
    auto it = std::find(this->_cameraEntities.begin(), this->_cameraEntities.end(), cameraId);
    if (it != this->_cameraEntities.end()) {
        this->_componentRegistry.removeComponent<Transform>(cameraId);
        this->_componentRegistry.removeComponent<Camera>(cameraId);

        this->_cameraEntities.erase(it);
    }
}

void CameraManager::setActiveCamera(EntityID id)
{
    for (size_t i = 0; i < this->_cameraEntities.size(); ++i) {
        if (this->_cameraEntities[i] == id) {
            this->_activeCameraIndex = static_cast<int>(i);
            this->_activeCamera = id;
            return;
        }
    }
}

void CameraManager::switchCamera()
{
    if (this->_cameraEntities.empty() || this->_activeCamera == INVALID_ENTITY)
        return;
    this->_activeCameraIndex = (this->_activeCameraIndex + 1) % static_cast<int>(this->_cameraEntities.size());
    this->_activeCamera = this->_cameraEntities[this->_activeCameraIndex];
}

void CameraManager::focusTarget(EntityID camEntity)
{
    Camera *cam = this->_componentRegistry.getComponent<Camera>(camEntity);
    Transform *transform = this->_componentRegistry.getComponent<Transform>(camEntity);
    if (!cam || !transform)
        return;

    cam->focusMode = true;
    float distance = glm::length(transform->position - cam->target);

    if (distance < cam->minDistance || distance > cam->maxDistance)
        distance = glm::clamp(distance, cam->minDistance, cam->maxDistance);

    transform->position = cam->target - cam->forward * distance;
}

void CameraManager::zoom(float dir)
{
    this->_cameraSystem.zoom(this->_activeCamera, dir);
}

void CameraManager::rotate(const glm::vec2 vect)
{
    this->_cameraSystem.rotate(this->_activeCamera, vect);
}

void CameraManager::pan(const glm::vec3 vect)
{
    this->_cameraSystem.pan(this->_activeCamera, vect);
}

void CameraManager::update(int viewportWidth, int viewportHeight)
{
    this->_cameraSystem.update(this->_cameraEntities, viewportWidth, viewportHeight);
}

EntityManager &CameraManager::getEntityManager() const
{
    return this->_entityManager;
}

ComponentRegistry &CameraManager::getComponentRegistry() const
{
    return this->_componentRegistry;
}

Camera *CameraManager::getActiveCamera() const
{
    if (this->_cameraEntities.empty())
    return nullptr;
    return this->_componentRegistry.getComponent<Camera>(this->_cameraEntities[this->_activeCameraIndex]);
}

EntityID CameraManager::getActiveCameraId() const
{
    if (this->_cameraEntities.empty())
    return 0;
    return this->_cameraEntities[this->_activeCameraIndex];
}

EntityID CameraManager::getCameraAtIndex(int index) const
{
    if (index < 0 || index >= static_cast<int>(this->_cameraEntities.size()))
        return INVALID_ENTITY;
    return this->_cameraEntities[index];
}

void CameraManager::toggleProjection(EntityID cameraId)
{
    Camera* cam = this->_componentRegistry.getComponent<Camera>(cameraId);
    if (!cam) return;

    cam->isOrtho = !cam->isOrtho;
}

void CameraManager::setProjection(EntityID cameraId, bool isOrtho)
{
    Camera* cam = this->_componentRegistry.getComponent<Camera>(cameraId);
    if (!cam) return;

    cam->isOrtho = isOrtho;
}