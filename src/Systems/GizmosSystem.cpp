#include "Systems/GizmosSystem.hpp"

GizmosSystem::GizmosSystem(
    ComponentRegistry& componentRegistry,
    EntityManager& entityManager,
    SelectionSystem& selectionSystem,
    CameraManager& cameraManager
) : _componentRegistry(componentRegistry),
    _entityManager(entityManager),
    _selectionSystem(selectionSystem),
    _cameraManager(cameraManager)
{}

void GizmosSystem::update()
{
    EntityID selectedEntity = this->_selectionSystem.getSelectedEntity();

    if (selectedEntity == INVALID_ENTITY) {
        this->_gizmo.isActive = false;
        return;
    }

    Transform* transform = this->_componentRegistry.getComponent<Transform>(selectedEntity);
    if (!transform) {
        this->_gizmo.isActive = false;
        return;
    }

    this->_gizmo.position = transform->position;
    this->_gizmo.isActive = true;
}

void GizmosSystem::render()
{
    if (!this->_gizmo.isActive)
        return;

    Camera* activeCamera = this->_cameraManager.getActiveCamera();
    if (!activeCamera)
        return;

    ofCamera cam;
    cam.setPosition(activeCamera->viewMatrix[3]);

    this->_gizmo.render(cam);
}

void Gizmo::render(const ofCamera& cam)
{
    if (!isActive)
        return;

    ofPushMatrix();
    ofTranslate(position);

    float arrowLength = size;
    float arrowHeadSize = size * 0.1f;
    float radius = size * 0.02f;

    ofSetColor(ofColor::red);
    ofPushMatrix();
    ofRotateDeg(90, 0, 0, 1);
    ofDrawCylinder(0, -arrowLength * 0.5f, 0, radius, arrowLength);
    ofTranslate(0, -arrowLength, 0);
    ofDrawCone(arrowHeadSize, arrowHeadSize * 2);
    ofPopMatrix();

    ofSetColor(ofColor::green);
    ofPushMatrix();
    ofDrawCylinder(0, arrowLength * 0.5f, 0, radius, arrowLength);
    ofTranslate(0, arrowLength, 0);
    ofRotateDeg(-180, 1, 0, 0);
    ofDrawCone(arrowHeadSize, arrowHeadSize * 2);
    ofPopMatrix();

    ofSetColor(ofColor::blue);
    ofPushMatrix();
    ofRotateDeg(-90, 1, 0, 0);
    ofDrawCylinder(0, -arrowLength * 0.5f, 0, radius, arrowLength);
    ofTranslate(0, -arrowLength, 0);
    ofDrawCone(arrowHeadSize, arrowHeadSize * 2);
    ofPopMatrix();

    ofPopMatrix();
}

bool Gizmo::isHovered(const ofCamera& cam, const glm::vec2& mousePos)
{
    glm::vec3 worldX = position + glm::vec3(size, 0, 0);
    glm::vec3 screenX = cam.worldToScreen(worldX);
    glm::vec3 screenPos = cam.worldToScreen(position);

    float distX = glm::distance(glm::vec2(screenPos), glm::vec2(screenX));
    float distMouse = glm::distance(glm::vec2(screenX), mousePos);

    return (distMouse < 10);
}
