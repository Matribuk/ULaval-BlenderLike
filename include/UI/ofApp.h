#pragma once

// Components
#include "Components/Primitive/Box.hpp"
#include "Components/Primitive/Plane.hpp"
#include "Components/Primitive/Sphere.hpp"
#include "Components/Camera.hpp"
#include "Components/Renderable.hpp"
#include "Components/Transform.hpp"

// Core
#include "Core/ComponentRegistry/ComponentRegistry.hpp"
#include "Core/EntityManager/EntityManager.hpp"

// Events
#include "Events/EventBridge/EventBridge.hpp"
#include "Events/EventManager/EventManager.hpp"
#include "Events/EventTypes/Events/CameraEvent.hpp"
#include "Events/EventTypes/Events/KeyEvent.hpp"
#include "Events/EventTypes/Events/MouseEvent.hpp"
#include "Events/EventTypes/Events/SelectionEvent.hpp"

// Managers
#include "Manager/FileManager/FileManager.hpp"
#include "Manager/InputManager/InputManager.hpp"
#include "Manager/ViewportManager/ViewportManager.hpp"
#include "Manager/HistoryManager/HistoryManager.hpp"
#include "Manager/ActionManager/ActionManager.hpp"
#include "Manager/PropertiesManager/PropertiesManager.hpp"
#include "Manager/SceneManager/SceneManager.hpp"
#include "Manager/UIManager/UIManager.hpp"

// Systems
#include "Systems/CameraSystem/CameraSystem.hpp"
#include "Systems/PrimitiveSystem/PrimitiveSystem.hpp"
#include "Systems/RenderSystem/RenderSystem.hpp"
#include "Systems/TransformSystem/TransformSystem.hpp"
#include "Systems/SelectionSystem/SelectionSystem.hpp"

// UI
#include "UI/ColorPalette/ColorPalette.hpp"
#include "UI/Toolbar/Toolbar.hpp"
#include "UI/Viewport/Viewport.hpp"
#include "UI/TranformPanel/TranformPanel.hpp"
#include "UI/MaterialPanel/MaterialPanel.hpp"
#include "UI/SkyboxPanel/SkyboxPanel.hpp"
#include "UI/InstructionsPanel/InstructionsPanel.hpp"
#include "UI/EventLogPanel/EventLogPanel.hpp"
#include "UI/AssetsPanel/AssetsPanel.hpp"

#include <ofMain.h>
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


    private:
        ofLight _light;
        EventManager _eventManager;
        EntityManager _entityManager;

        std::unique_ptr<UIManager> _uiManager;
        std::unique_ptr<PropertiesManager> _propertiesManager;
        std::unique_ptr<ActionManager> _actionManager;
        std::unique_ptr<HistoryManager> _historyManager;
        std::unique_ptr<ViewportManager> _viewportManager;
        std::unique_ptr<CameraManager> _cameraManager;
        std::unique_ptr<FileManager> _fileManager;
        std::unique_ptr<SceneManager> _sceneManager;

        std::unique_ptr<TranformPanel> _tranformPanel;
        std::unique_ptr<MaterialPanel> _materialPanel;
        std::unique_ptr<ColorPalette> _colorPalette;
        std::unique_ptr<EventBridge> _eventBridge;
        std::unique_ptr<SkyboxPanel> _skyboxPanel;
        std::unique_ptr<InstructionsPanel> _instructionsPanel;
        std::unique_ptr<EventLogPanel> _eventLogPanel;
        std::unique_ptr<AssetsPanel> _assetsPanel;
        std::unique_ptr<Toolbar> _toolbar;
        ComponentRegistry _componentRegistry;

        std::unique_ptr<TransformSystem> _transformSystem;
        std::unique_ptr<CameraSystem> _cameraSystem;
        std::unique_ptr<RenderSystem> _renderSystem;
        std::unique_ptr<PrimitiveSystem> _primitiveSystem;
        std::unique_ptr<SelectionSystem> _selectionSystem;
        ofxImGui::Gui _gui;
        std::vector<EntityID> _testEntities;
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

        void _setupEventSubscribers();
        void _setupSystems();
        void _setupScene();
        void _drawUI();
};
