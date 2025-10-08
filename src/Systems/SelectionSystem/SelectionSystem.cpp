#include "Systems/SelectionSystem/SelectionSystem.hpp"
#include "UI/Viewport/Viewport.hpp"
#include <limits>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

SelectionSystem::SelectionSystem(
    ComponentRegistry& registry,
    EntityManager& entityManager,
    EventManager& eventManager,
    CameraManager& cameraManager,
    ViewportManager& viewportManager
)
    : _componentRegistry(registry),
      _entityManager(entityManager),
      _eventManager(eventManager),
      _cameraManager(cameraManager),
      _viewportManager(viewportManager)
{
}

void SelectionSystem::setup()
{
    _eventManager.subscribe<MouseEvent>([this](const MouseEvent& e) {
        if (e.type == MouseEventType::Pressed && e.button == 0) {
            _handleMouseEvent(e);
        }
    });
}

EntityID SelectionSystem::getSelectedEntity()
{
    return _selectedEntity;
}

void SelectionSystem::setSelectedEntity(EntityID selectedEntity)
{
    _selectedEntity = selectedEntity;
}

void SelectionSystem::_handleMouseEvent(const MouseEvent& e)
{
    glm::vec2 mousePos(static_cast<float>(e.x), static_cast<float>(e.y));
    _performRaycastInActiveViewport(mousePos);
}

EntityID SelectionSystem::_performRaycastInActiveViewport(const glm::vec2& mouseGlobalPos)
{
    // 1) Get active viewport
    Viewport* vp = nullptr;
    try { vp = _viewportManager.getActiveViewport(); } catch(...) { vp = nullptr; }
    if (!vp) return INVALID_ENTITY;

    // 2) Viewport rect
    ofRectangle rect;
    try { rect = vp->getRect(); } catch(...) { return INVALID_ENTITY; }

    // 3) Check if click is inside viewport
    if (!rect.inside(mouseGlobalPos.x, mouseGlobalPos.y)) {
        std::cout << "[SelectionSystem] Click outside viewport -> ignored\n";
        return INVALID_ENTITY;
    }

    // 4) Local viewport coordinates
    float localX = mouseGlobalPos.x - rect.x;
    float localY = mouseGlobalPos.y - rect.y;
    int vpWidth = static_cast<int>(rect.getWidth());
    int vpHeight = static_cast<int>(rect.getHeight());
    if (vpWidth <= 0 || vpHeight <= 0) return INVALID_ENTITY;

    // 5) Get camera
    EntityID camEntityId = vp->getCamera();
    if (camEntityId == INVALID_ENTITY) camEntityId = _cameraManager.getActiveCameraId();
    if (camEntityId == INVALID_ENTITY) return INVALID_ENTITY;

    Camera* cam = _componentRegistry.getComponent<Camera>(camEntityId);
    Transform* camTransform = _componentRegistry.getComponent<Transform>(camEntityId);
    if (!cam || !camTransform) return INVALID_ENTITY;

    glm::mat4 proj = cam->projMatrix;
    glm::mat4 view = cam->viewMatrix;
    if (fabs(glm::determinant(proj * view)) < 1e-8f) return INVALID_ENTITY;

    glm::mat4 invVP;
    try { invVP = glm::inverse(proj * view); } catch(...) { return INVALID_ENTITY; }

    // 6) Convert mouse to world ray
    float ndcX = (localX / static_cast<float>(vpWidth)) * 2.0f - 1.0f;
    float ndcY = 1.0f - (localY / static_cast<float>(vpHeight)) * 2.0f;
    glm::vec4 clipNear(ndcX, ndcY, -1.0f, 1.0f);
    glm::vec4 clipFar(ndcX, ndcY, 1.0f, 1.0f);

    glm::vec4 worldNear4 = invVP * clipNear;
    glm::vec4 worldFar4  = invVP * clipFar;

    if (fabs(worldNear4.w) < 1e-8f || fabs(worldFar4.w) < 1e-8f) return INVALID_ENTITY;

    glm::vec3 rayOrigin = glm::vec3(worldNear4) / worldNear4.w;
    glm::vec3 rayDir    = glm::normalize(glm::vec3(worldFar4) / worldFar4.w - rayOrigin);

    // 7) Test intersection with all selectable entities
    EntityID closest = INVALID_ENTITY;
    float closestT = std::numeric_limits<float>::max();

    for (EntityID id : _entityManager.getAllEntities()) {
        if (id == INVALID_ENTITY || _componentRegistry.hasComponent<Camera>(id)) continue;

        Transform* t = _componentRegistry.getComponent<Transform>(id);
        Selectable* s = _componentRegistry.getComponent<Selectable>(id);
        if (!t || !s) continue;

        glm::vec3 aabbMin, aabbMax;

        if (Box* box = _componentRegistry.getComponent<Box>(id)) {
            glm::vec3 half = box->dimensions * 0.5f;
            aabbMin = t->position - half;
            aabbMax = t->position + half;
        } else if (Sphere* sphere = _componentRegistry.getComponent<Sphere>(id)) {
            glm::vec3 half(sphere->radius);
            aabbMin = t->position - half;
            aabbMax = t->position + half;
        } else if (Plane* plane = _componentRegistry.getComponent<Plane>(id)) {
            glm::vec3 half(plane->size.x*0.5f, 0.01f, plane->size.y*0.5f); // thin Y thickness
            aabbMin = t->position - half;
            aabbMax = t->position + half;
        } else {
            glm::vec3 half = glm::max(t->scale * 0.5f, glm::vec3(0.0001f));
            aabbMin = t->position - half;
            aabbMax = t->position + half;
        }

        float tHit = 0.0f;
        bool hit = false;
        try { hit = _intersectsRayAABB(rayOrigin, rayDir, aabbMin, aabbMax, tHit); } 
        catch(...) { hit = false; }

        if (hit && tHit < closestT) {
            closestT = tHit;
            closest = id;
        }
    }

    // 8) Update selection and log meaningful info
    if (closest != INVALID_ENTITY) {
        std::cout << "[SelectionSystem] Entity " << closest << " selected\n";
        this->_eventManager.emit(SelectionEvent(closest, true));
        this->setSelectedEntity(closest);
    } else {
        std::cout << "[SelectionSystem] No primitive touched\n";
    }

    _updateSelection(closest);
    return closest;
}

void SelectionSystem::_updateSelection(EntityID selected)
{
    // Validate selected exists
    if (selected != INVALID_ENTITY && !_entityManager.isEntityValid(selected)) {
        selected = INVALID_ENTITY;
    }

    std::vector<EntityID> changes;

    for (EntityID id : _entityManager.getAllEntities()) {
        Selectable* s = _componentRegistry.getComponent<Selectable>(id);
        if (!s) continue;

        bool nowSelected = (id == selected);
        if (s->isSelected != nowSelected) {
            s->isSelected = nowSelected;
            changes.push_back(id);
        }
    }

    for (EntityID id : changes) {
        try {
            if (_componentRegistry.getComponent<Selectable>(id)->isSelected) {
                _eventManager.emit(SelectionEvent(id, true));
            } else {
                _eventManager.emit(SelectionEvent(id, false));
            }
        } catch (...) {}
    }

    if (changes.empty() && selected == INVALID_ENTITY) {
        _eventManager.emit(SelectionEvent(INVALID_ENTITY, false));
    }
}

bool SelectionSystem::_intersectsRayAABB(
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
