#pragma once

#include "Core/ApplicationBootstrapper.hpp"

#include "Events/EventBridge.hpp"
#include "Events/EventTypes/KeyEvent.hpp"
#include "Events/EventTypes/MouseEvent.hpp"
#include "Events/EventTypes/SelectionEvent.hpp"
#include "Events/EventTypes/CameraEvent.hpp"

#include "Components/Transform.hpp"

#include <sstream>
#include <iostream>

class ApplicationRuntime {
    public:
        ApplicationRuntime(
            EventManager& eventManager,
            ComponentRegistry& componentRegistry,
            SystemsContext& systems,
            ManagersContext& managers,
            UIContext& ui,
            std::vector<EntityID>& testEntities
        );

        ~ApplicationRuntime();

        void initialize();
        void update(int windowWidth, int windowHeight);
        void shutdown();

    private:
        EventManager& _eventManager;
        ComponentRegistry& _componentRegistry;
        SystemsContext& _systems;
        ManagersContext& _managers;
        UIContext& _ui;

        std::vector<EntityID>& _testEntities;
        std::unique_ptr<EventBridge> _eventBridge;

        void _setupEventSubscribers();
};
