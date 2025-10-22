#include "Systems/EyedropperSystem.hpp"
#include "Manager/ViewportManager.hpp"
#include "UI/Viewport.hpp"
#include "Events/EventTypes/EyedropperCancelledEvent.hpp"

EyedropperSystem::EyedropperSystem(
    ComponentRegistry& registry,
    EntityManager& entityManager,
    EventManager& eventManager
)
    : _componentRegistry(registry),
      _entityManager(entityManager),
      _eventManager(eventManager)
{
}

void EyedropperSystem::setupManagers(CameraManager& cameraManager, ViewportManager& viewportManager)
{
    this->_cameraManager = &cameraManager;
    this->_viewportManager = &viewportManager;

    this->_eventManager.subscribe<MouseEvent>([this](const MouseEvent& e) {
        if (e.type == MouseEventType::Moved) {
            this->_handleMouseMove(e);
        } else if (e.type == MouseEventType::Pressed && e.button == 0) {
            this->_handleMousePressed(e);
        }
    });
}

void EyedropperSystem::setEyedropperMode(bool activateEyedropperMode)
{
    this->_isEyedropperMode = activateEyedropperMode;
}

void EyedropperSystem::_handleMouseMove(const MouseEvent& e)
{
    if (!this->_isEyedropperMode) return;

    glm::vec2 mousePos(static_cast<float>(e.x), static_cast<float>(e.y));
    EntityID entity = this->_performRaycast(mousePos);

    if (entity != INVALID_ENTITY) {
        Renderable* r = this->_componentRegistry.getComponent<Renderable>(entity);
        if (r) {
            this->_eventManager.emit(ColorPreviewEvent(r->color, entity, true));
            return;
        }
    }

    this->_eventManager.emit(ColorPreviewEvent(ofColor::white, INVALID_ENTITY, false));
}

void EyedropperSystem::_handleMousePressed(const MouseEvent& e)
{
    if (!this->_isEyedropperMode) return;

    glm::vec2 mousePos(static_cast<float>(e.x), static_cast<float>(e.y));
    EntityID entity = this->_performRaycast(mousePos);

    if (entity != INVALID_ENTITY) {
        Renderable* r = this->_componentRegistry.getComponent<Renderable>(entity);
        if (r) {
            this->_eventManager.emit(ColorPickedEvent(r->color, entity));
        }
    } else {
        this->_isEyedropperMode = false;
        this->_eventManager.emit(EyedropperCancelledEvent());
    }
}

glm::mat4 EyedropperSystem::_getOrComputeTransformMatrix(Transform* t) const
{
    if (!t->isDirty && glm::determinant(t->matrix) != 0.0f) {
        return t->matrix;
    }

    glm::mat4 matrice = glm::translate(glm::mat4(1.0f), t->position);
    matrice = matrice * glm::yawPitchRoll(t->rotation.y, t->rotation.x, t->rotation.z);
    matrice = glm::scale(matrice, t->scale);

    return matrice;
}

void EyedropperSystem::_transformAABB(const glm::vec3& localMin, const glm::vec3& localMax,
                                      const glm::mat4& transform,
                                      glm::vec3& outMin, glm::vec3& outMax) const
{
    glm::vec3 corners[8] = {
        glm::vec3(localMin.x, localMin.y, localMin.z),
        glm::vec3(localMax.x, localMin.y, localMin.z),
        glm::vec3(localMin.x, localMax.y, localMin.z),
        glm::vec3(localMax.x, localMax.y, localMin.z),
        glm::vec3(localMin.x, localMin.y, localMax.z),
        glm::vec3(localMax.x, localMin.y, localMax.z),
        glm::vec3(localMin.x, localMax.y, localMax.z),
        glm::vec3(localMax.x, localMax.y, localMax.z)
    };

    outMin = glm::vec3(std::numeric_limits<float>::max());
    outMax = glm::vec3(std::numeric_limits<float>::lowest());

    for (int i = 0; i < 8; ++i) {
        glm::vec4 transformed = transform * glm::vec4(corners[i], 1.0f);
        glm::vec3 point = glm::vec3(transformed) / transformed.w;

        outMin = glm::min(outMin, point);
        outMax = glm::max(outMax, point);
    }
}

EntityID EyedropperSystem::_performRaycast(const glm::vec2& mouseGlobalPos)
{
    Viewport* vp = nullptr;
    try { vp = this->_viewportManager->getActiveViewport(); } catch(...) { vp = nullptr; }
    if (!vp) return INVALID_ENTITY;

    ofRectangle rect;
    try { rect = vp->getRect(); } catch(...) { return INVALID_ENTITY; }

    if (!rect.inside(mouseGlobalPos.x, mouseGlobalPos.y)) return INVALID_ENTITY;

    float localX = mouseGlobalPos.x - rect.x;
    float localY = mouseGlobalPos.y - rect.y;
    int vpWidth = static_cast<int>(rect.getWidth());
    int vpHeight = static_cast<int>(rect.getHeight());

    if (vpWidth <= 0 || vpHeight <= 0) return INVALID_ENTITY;

    EntityID camEntityId = vp->getCamera();
    if (camEntityId == INVALID_ENTITY) camEntityId = this->_cameraManager->getActiveCameraId();
    if (camEntityId == INVALID_ENTITY) return INVALID_ENTITY;

    Camera* cam = this->_componentRegistry.getComponent<Camera>(camEntityId);
    Transform* camTransform = this->_componentRegistry.getComponent<Transform>(camEntityId);
    if (!cam || !camTransform) return INVALID_ENTITY;

    glm::mat4 proj;
    if (fabs(glm::determinant(cam->projMatrix)) > 1e-8f) {
        proj = cam->projMatrix;
    } else {
        float viewportAspect = static_cast<float>(vpWidth) / static_cast<float>(vpHeight);
        if (cam->isOrtho) {
            float halfWidth = cam->orthoScale * viewportAspect;
            float halfHeight = cam->orthoScale;
            proj = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, cam->nearClip, cam->farClip);
        } else {
            proj = glm::perspective(glm::radians(cam->fov), viewportAspect, cam->nearClip, cam->farClip);
        }
    }

    glm::vec3 camPos = camTransform->position;
    glm::vec3 forward = glm::normalize(cam->forward);
    glm::vec3 upVec = glm::normalize(-cam->up);
    glm::mat4 view = glm::lookAt(camPos, camPos + forward, upVec);

    if (fabs(glm::determinant(proj * view)) < 1e-8f) return INVALID_ENTITY;

    glm::mat4 invVP;
    try { invVP = glm::inverse(proj * view); } catch(...) { return INVALID_ENTITY; }

    float ndcX = 1.0f - (localX / static_cast<float>(vpWidth)) * 2.0f;
    float ndcY = (localY / static_cast<float>(vpHeight)) * 2.0f - 1.0f;

    glm::vec4 clipNear(ndcX, ndcY, -1.0f, 1.0f);
    glm::vec4 clipFar(ndcX, ndcY, 1.0f, 1.0f);

    glm::vec4 worldNear4 = invVP * clipNear;
    glm::vec4 worldFar4  = invVP * clipFar;

    if (fabs(worldNear4.w) < 1e-8f || fabs(worldFar4.w) < 1e-8f) return INVALID_ENTITY;

    glm::vec3 rayOrigin = glm::vec3(worldNear4) / worldNear4.w;
    glm::vec3 rayDir    = glm::normalize(glm::vec3(worldFar4) / worldFar4.w - rayOrigin);

    EntityID closest = INVALID_ENTITY;
    float closestT = std::numeric_limits<float>::max();

    for (EntityID id : this->_entityManager.getAllEntities()) {
        if (id == INVALID_ENTITY || this->_componentRegistry.hasComponent<Camera>(id)) continue;

        Transform* t = this->_componentRegistry.getComponent<Transform>(id);
        Renderable* r = this->_componentRegistry.getComponent<Renderable>(id);
        if (!t || !r) continue;

        glm::mat4 transformMatrix = _getOrComputeTransformMatrix(t);
        glm::vec3 localMin, localMax;

        if (Box* box = this->_componentRegistry.getComponent<Box>(id)) {
            glm::vec3 half = box->dimensions * 0.5f;
            localMin = -half;
            localMax = half;
        } else if (Sphere* sphere = this->_componentRegistry.getComponent<Sphere>(id)) {
            glm::vec3 half(sphere->radius);
            localMin = -half;
            localMax = half;
        } else if (Plane* plane = this->_componentRegistry.getComponent<Plane>(id)) {
            glm::vec3 half(plane->size.x*0.5f, 0.5f, plane->size.y*0.5f);
            localMin = -half;
            localMax = half;
        } else {
            glm::vec3 half = glm::max(t->scale * 0.5f, glm::vec3(0.1f));
            localMin = -half;
            localMax = half;
        }

        glm::vec3 worldMin, worldMax;
        this->_transformAABB(localMin, localMax, transformMatrix, worldMin, worldMax);

        float tHit = 0.0f;
        bool hit = this->_intersectsRayAABB(rayOrigin, rayDir, worldMin, worldMax, tHit);

        if (hit && tHit < closestT) {
            closestT = tHit;
            closest = id;
        }
    }

    return closest;
}

bool EyedropperSystem::_intersectsRayAABB(
    const glm::vec3& rayOrigin, const glm::vec3& rayDir,
    const glm::vec3& aabbMin, const glm::vec3& aabbMax, float& outT) const
{
    float tmin = -std::numeric_limits<float>::infinity();
    float tmax = std::numeric_limits<float>::infinity();

    for (int i = 0; i < 3; ++i) {
        if (fabs(rayDir[i]) < 1e-8f) {
            if (rayOrigin[i] < aabbMin[i] || rayOrigin[i] > aabbMax[i]) return false;
        } else {
            float invD = 1.0f / rayDir[i];
            float t1 = (aabbMin[i] - rayOrigin[i]) * invD;
            float t2 = (aabbMax[i] - rayOrigin[i]) * invD;
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            if (tmin > tmax) return false;
        }
    }

    if (tmax < 0.0f) return false;
    outT = tmin >= 0.0f ? tmin : tmax;
    return true;
}
