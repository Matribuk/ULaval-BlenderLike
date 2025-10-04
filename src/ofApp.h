#pragma once

#include "ofMain.h"
#include "./Events/EventManager/EventManager.hpp"
#include "./Events/EventBridge/EventBridge.hpp"
#include "./Events/EventTypes/Events/MouseEvent.hpp"
#include "./Events/EventTypes/Events/KeyEvent.hpp"
#include "./Events/EventTypes/Events/CameraEvent.hpp"
#include "./Events/EventTypes/Events/SelectionEvent.hpp"
#include "./Manager/InputManager/InputManager.hpp"
#include "./Manager/ViewportManager/ViewportManager.hpp"
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
#include "./UI/ToolBar/ToolBar.hpp"
#include "./UI/Viewport/Viewport.hpp"
#include "./UI/ColorPalette/ColorPalette.hpp"
#include "ofxImGui.h"

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
    EventManager _eventManager;

    std::unique_ptr<ColorPalette> _colorPalette;
    std::unique_ptr<EventBridge> _eventBridge;
    std::unique_ptr<Toolbar> _toolbar;
    std::unique_ptr<ViewportManager> _viewportManager;
    EntityManager _entityManager;
    ComponentRegistry _componentRegistry;

    std::unique_ptr<TransformSystem> _transformSystem;
    std::unique_ptr<CameraSystem> _cameraSystem;
    std::unique_ptr<RenderSystem> _renderSystem;
    std::unique_ptr<PrimitiveSystem> _primitiveSystem;

    struct EventLog {
        std::string message;
        std::chrono::time_point<std::chrono::steady_clock> timestamp;
        ofColor color;
    };

    std::vector<EventLog> _eventLogs;
    const size_t _MAX_LOGS = 20;

    ofxImGui::Gui _gui;
    std::vector<EntityID> _testEntities;
    EntityID _selectedEntity = 0;
    EntityID _cameraEntity = INVALID_ENTITY;

    glm::vec3 _cameraPosition{0.0f, 5.0f, 10.0f};
    glm::vec3 _cameraTarget{0.0f, 0.0f, 0.0f};

    int _keyPressCount = 0;
    int _keyReleaseCount = 0;
    int _mousePressCount = 0;
    int _mouseReleaseCount = 0;
    int _mouseMoveCount = 0;
    int _selectionEventCount = 0;
    int _cameraEventCount = 0;

    void _addLog(const std::string& message, const ofColor& color = ofColor::white);
    void _setupEventSubscribers();
    void _setupShortcuts();
    void _setupSystems();
    void _setupScene();
    void _testEntitySystem();
    void _drawUI();
    void _drawStats();
    void _drawEventLog();
    void _drawInstructions();
    void _drawEntityList();
};
