#pragma once

#include "Core/EntityManager.hpp"
#include "Core/ComponentRegistry.hpp"
#include "Core/ApplicationBootstrapper.hpp"
#include "Core/ApplicationRuntime.hpp"

#include "Events/EventManager.hpp"
#include "Events/EventTypes/KeyEvent.hpp"

#include <memory>
#include <ofMain.h>
#include "ofxImGui.h"

class ofApp : public ofBaseApp {
    public:
        void setup() override;
        void update() override;
        void draw() override;
        void exit() override;

        void keyPressed(int key) override;
        void keyReleased(int key) override;

    private:
        EventManager _eventManager;
        EntityManager _entityManager;
        ComponentRegistry _componentRegistry;

        std::unique_ptr<ApplicationBootstrapper> _bootstrapper;
        std::unique_ptr<ApplicationRuntime> _runtime;

        ofLight _light;
        ofxImGui::Gui _gui;

        void _setupOpenGL();
        void _drawUI();
};
