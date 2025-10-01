#pragma once

#include "ofMain.h"
#include "Core/SceneManager/SceneManager.hpp"
#include "Events/EventBridge/EventBridge.hpp"
#include "Events/EventManager/EventManager.hpp"

#include "Core/ComponentRegistry/ComponentRegistry.hpp"
#include "Core/EntityManager/EntityManager.hpp"
#include "Systems/RenderSystem/RenderSystem.hpp"
#include "Systems/TransformSystem/TransformSystem.hpp"

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;
		void exit() override;

		void keyPressed(int key) override;
		void keyReleased(int key) override;
		void mouseMoved(int x, int y) override;
		void mouseDragged(int x, int y, int button) override;
		void mousePressed(int x, int y, int button) override;
		void mouseReleased(int x, int y, int button) override;
		void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
		void mouseEntered(int x, int y) override;
		void mouseExited(int x, int y) override;
		void windowResized(int w, int h) override;
		void dragEvent(ofDragInfo dragInfo) override;
		void gotMessage(ofMessage msg) override;

	private:
		std::unique_ptr<SceneManager> sceneManager;
		std::unique_ptr<EventManager> eventManager;
		std::unique_ptr<EventBridge> eventBridge;

		ComponentRegistry registry;
		EntityManager entityManager;
		std::unique_ptr<RenderSystem> renderSystem;
		std::unique_ptr<TransformSystem> transformSystem;

		EntityID cubeEntity;
		EntityID cameraEntity;
};
