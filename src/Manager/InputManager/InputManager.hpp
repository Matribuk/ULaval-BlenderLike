#pragma once

#include <unordered_map>
#include <functional>
#include <glm/vec2.hpp>

class InputManager {
    public:
        static InputManager& get();

        bool isKeyPressed(int key) const;
        bool wasKeyJustPressed(int key) const;
        bool wasKeyJustReleased(int key) const;

        glm::vec2 getMousePosition() const;
        glm::vec2 getMouseDelta() const;
        bool isMouseButtonPressed(int button) const;

        void registerShortcut(const std::vector<int>& keys, std::function<void()> action);
        void processShortcuts();

        void onKeyPressed(int key);
        void onKeyReleased(int key);
        void onMouseMoved(float x, float y);
        void onMousePressed(int button);
        void onMouseReleased(int button);

        void endFrame();

    private:
        InputManager() = default;

        std::unordered_map<int, bool> _keyStates;
        std::unordered_map<int, bool> _prevKeyStates;

        std::unordered_map<int, bool> _mouseButtonStates;
        std::unordered_map<int, bool> _prevMouseButtonStates;

        glm::vec2 _mousePos { 0, 0 };
        glm::vec2 _prevMousePos { 0, 0 };

        std::vector<std::pair<std::vector<int>, std::function<void()>>> _shortcuts;
};
