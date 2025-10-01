#pragma once

#include "ofMain.h"
#include "./Events/EventManager/EventManager.hpp"
#include "./Events/EventBridge/EventBridge.hpp"
#include "./Events/EventTypes/Events/MouseEvent.hpp"
#include "./Events/EventTypes/Events/KeyEvent.hpp"
#include "./Events/EventTypes/Events/CameraEvent.hpp"
#include "./Events/EventTypes/Events/SelectionEvent.hpp"
#include "./Manager/InputManager/InputManager.hpp"
#include "./Core/EntityManager/EntityManager.hpp"
#include "./Core/ComponentRegistry/ComponentRegistry.hpp"
#include "./Systems/TransformSystem/TransformSystem.hpp"
#include "./Systems/CameraSystem/CameraSystem.hpp"
#include "./Systems/RenderSystem/RenderSystem.hpp"
#include "./Systems/PrimitiveSystem/PrimitiveSystem.hpp"
#include "./Components/Transform.hpp"
#include "./Components/Camera.hpp"
#include "./Components/Renderable.hpp"
#include "./Components/Primitive/Box.hpp"
#include "./Components/Primitive/Sphere.hpp"
#include "./Components/Primitive/Plane.hpp"
#include <sstream>

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseScrolled(int x, int y, float scrollX, float scrollY);

private:
    EventManager eventManager;
    std::unique_ptr<EventBridge> eventBridge;
    EntityManager entityManager;
    ComponentRegistry componentRegistry;

    // Systems
    std::unique_ptr<TransformSystem> transformSystem;
    std::unique_ptr<CameraSystem> cameraSystem;
    std::unique_ptr<RenderSystem> renderSystem;
    std::unique_ptr<PrimitiveSystem> primitiveSystem;

    struct EventLog {
        std::string message;
        std::chrono::time_point<std::chrono::steady_clock> timestamp;
        ofColor color;
    };
    
    std::vector<EventLog> eventLogs;
    const size_t MAX_LOGS = 20;

    std::vector<EntityID> testEntities;
    EntityID selectedEntity = 0;
    EntityID cameraEntity = INVALID_ENTITY;

    glm::vec3 cameraPosition{0.0f, 5.0f, 10.0f};
    glm::vec3 cameraTarget{0.0f, 0.0f, 0.0f};

    int _keyPressCount = 0;
    int _keyReleaseCount = 0;
    int _mousePressCount = 0;
    int _mouseReleaseCount = 0;
    int _mouseMoveCount = 0;
    int _selectionEventCount = 0;
    int _cameraEventCount = 0;

    void addLog(const std::string& message, const ofColor& color = ofColor::white);
    void setupEventSubscribers();
    void setupShortcuts();
    void setupSystems();
    void setupScene();
    void testEntitySystem();
    void drawUI();
    void drawStats();
    void drawEventLog();
    void drawInstructions();
    void drawEntityList();
};