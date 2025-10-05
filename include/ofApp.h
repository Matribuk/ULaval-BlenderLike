#pragma once

#include "ofMain.h"

#include "EventManager.hpp"
#include "EventBridge.hpp"
#include "MouseEvent.hpp"
#include "KeyEvent.hpp"
#include "CameraEvent.hpp"
#include "SelectionEvent.hpp"
#include "InputManager.hpp"
#include "ViewportManager.hpp"
#include "FileManager.hpp"
#include "EntityManager.hpp"
#include "ComponentRegistry.hpp"
#include "TransformSystem.hpp"
#include "CameraSystem.hpp"
#include "RenderSystem.hpp"
#include "PrimitiveSystem.hpp"
#include "Transform.hpp"
#include "Camera.hpp"
#include "Renderable.hpp"
#include "Box.hpp"
#include "Sphere.hpp"
#include "Plane.hpp"
#include "ToolBar.hpp"
#include "Viewport.hpp"
#include "ColorPalette.hpp"
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
        std::unique_ptr<HistoryManager> _historyManager;
        std::unique_ptr<FileManager> _fileManager;
        std::unique_ptr<EventBridge> _eventBridge;
        std::unique_ptr<Toolbar> _toolbar;
        std::unique_ptr<ViewportManager> _viewportManager;
        std::unique_ptr<CameraManager> _cameraManager;
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
        EntityID _cameraEntity2 = INVALID_ENTITY;

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
