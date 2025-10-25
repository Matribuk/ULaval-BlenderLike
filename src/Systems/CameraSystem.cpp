#include "Systems/CameraSystem.hpp"

CameraSystem::CameraSystem(ComponentRegistry& registry, TransformSystem& transformSystem)
    : _componentRegistry(registry), _transformSystem(transformSystem) {}

void CameraSystem::update(std::vector<EntityID> cameraEntities, int viewportWidth, int viewportHeight)
{
    for (EntityID id : cameraEntities)
    {
        Camera *cam = this->_componentRegistry.getComponent<Camera>(id);
        Transform *transform = this->_componentRegistry.getComponent<Transform>(id);
        if (cam && transform) {
            cam->aspectRatio = static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight);

            if (cam->focusMode && cam->targetEntity != INVALID_ENTITY) {
                Transform* targetTransform = this->_componentRegistry.getComponent<Transform>(cam->targetEntity);
                if (targetTransform) {
                    cam->target = targetTransform->position;

                    float cosYaw = cos(cam->yaw);
                    float sinYaw = sin(cam->yaw);
                    float cosPitch = cos(cam->pitch);
                    float sinPitch = sin(cam->pitch);

                    glm::vec3 offset(
                        -cam->distanceToTarget * cosPitch * sinYaw,
                        -cam->distanceToTarget * sinPitch,
                        -cam->distanceToTarget * cosPitch * cosYaw
                    );

                    transform->position = cam->target + offset;
                    transform->isDirty = true;
                    cam->forward = glm::normalize(cam->target - transform->position);
                }
            }

            if (cam->focusMode)
                cam->viewMatrix = glm::lookAt(transform->position, cam->target, cam->up);
            else
                cam->viewMatrix = glm::lookAt(transform->position, transform->position + cam->forward, cam->up);

            if (cam->isOrtho) {
                float halfWidth = cam->orthoScale * cam->aspectRatio;
                float halfHeight = cam->orthoScale;

                cam->projMatrix = glm::ortho(
                    -halfWidth, halfWidth,
                    -halfHeight, halfHeight,
                    cam->nearClip,
                    cam->farClip
                );
            } else {
                cam->projMatrix = glm::perspective(
                    glm::radians(cam->fov),
                    cam->aspectRatio,
                    cam->nearClip,
                    cam->farClip
                );
            }
        }
    }
}

void CameraSystem::pan(EntityID camEntity, glm::vec3 vect)
{
    Camera* cam = this->_componentRegistry.getComponent<Camera>(camEntity);
    if (!cam) return;

    if (cam->focusMode) return;

    this->_transformSystem.setCameraPosition(camEntity, vect * cam->panSensitivity);

    cam->forward = this->_transformSystem.getForward(camEntity);
    cam->up = this->_transformSystem.getUp(camEntity);
}

void CameraSystem::rotate(EntityID camEntity, glm::vec2 vect)
{
    Camera* cam = this->_componentRegistry.getComponent<Camera>(camEntity);
    Transform* transform = this->_componentRegistry.getComponent<Transform>(camEntity);
    if (!cam || !transform) return;

    if (cam->focusMode) {
        float deltaTime = ofGetLastFrameTime();

        float yawDelta = vect.x * cam->rotateSensitivity * deltaTime;
        float pitchDelta = vect.y * cam->rotateSensitivity * deltaTime;

        cam->yaw += yawDelta;
        cam->pitch += pitchDelta;

        cam->pitch = glm::clamp(cam->pitch, glm::radians(cam->minPitch), glm::radians(cam->maxPitch));

        float cosYaw = cos(cam->yaw);
        float sinYaw = sin(cam->yaw);
        float cosPitch = cos(cam->pitch);
        float sinPitch = sin(cam->pitch);

        glm::vec3 offset(
            -cam->distanceToTarget * cosPitch * sinYaw,
            -cam->distanceToTarget * sinPitch,
            -cam->distanceToTarget * cosPitch * cosYaw
        );

        transform->position = cam->target + offset;
        transform->isDirty = true;

        cam->forward = glm::normalize(cam->target - transform->position);
        cam->up = glm::vec3(0, 1, 0);
    } else {
        this->_transformSystem.setCameraRotation(camEntity, vect * cam->rotateSensitivity);

        cam->forward = this->_transformSystem.getForward(camEntity);
        cam->up = this->_transformSystem.getUp(camEntity);

        cam->yaw = atan2(cam->forward.x, cam->forward.z);
        cam->pitch = asin(cam->forward.y);
    }
}

void CameraSystem::zoom(EntityID camEntity, float amount)
{
    float deltaTime = ofGetLastFrameTime();
    Camera *cam = this->_componentRegistry.getComponent<Camera>(camEntity);
    Transform *transform = this->_componentRegistry.getComponent<Transform>(camEntity);
    if (!cam || !transform) return;

    if (cam->isOrtho) {
        float zoomFactor = 1.0f - (amount * deltaTime * 0.1f);
        cam->orthoScale *= zoomFactor;
        cam->orthoScale = glm::clamp(cam->orthoScale, cam->minOrthoScale, cam->maxOrthoScale);
    } else {
        float newFov = cam->fov - deltaTime * amount * cam->zoomSensitivity;
        newFov = glm::clamp(newFov, cam->minFov, cam->maxFov);
        cam->fov = newFov;
    }

    if (cam->focusMode) {
        cam->viewMatrix = glm::lookAt(
            transform->position,
            cam->target,
            cam->up
        );
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
