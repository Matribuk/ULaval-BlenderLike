#include "InputManager.hpp"


InputManager& InputManager::get()
{
    static InputManager instance;
    return instance;
}

bool InputManager::isKeyPressed(int key) const
{
    auto it = this->_keyStates.find(key);
    return it != this->_keyStates.end() && it->second;
}

bool InputManager::wasKeyJustPressed(int key) const {
    auto itNow = this->_keyStates.find(key);
    auto itPrev = this->_prevKeyStates.find(key);

    bool now = (itNow != this->_keyStates.end() && itNow->second);
    bool prev = (itPrev != this->_prevKeyStates.end() && itPrev->second);

    return now && !prev;
}

bool InputManager::wasKeyJustReleased(int key) const {
    auto itNow = this->_keyStates.find(key);
    auto itPrev = this->_prevKeyStates.find(key);

    bool now = (itNow != this->_keyStates.end() && itNow->second);
    bool prev = (itPrev != this->_prevKeyStates.end() && itPrev->second);

    return !now && prev;
}

glm::vec2 InputManager::getMousePosition() const
{
    return this->_mousePos;
}

glm::vec2 InputManager::getMouseDelta() const
{
    return this->_mousePos - this->_prevMousePos;
}

bool InputManager::isMouseButtonPressed(int button) const
{
    auto it = this->_mouseButtonStates.find(button);
    return it != this->_mouseButtonStates.end() && it->second;
}

void InputManager::registerShortcut(const std::vector<int>& keys, std::function<void()> action)
{
    this->_shortcuts.emplace_back(keys, action);
}

void InputManager::onKeyPressed(int key)
{
    this->_keyStates[key] = true;
}

void InputManager::onKeyReleased(int key)
{
    this->_keyStates[key] = false;
}

void InputManager::onMouseMoved(float x, float y)
{
    this->_prevMousePos = this->_mousePos;
    this->_mousePos = glm::vec2(x, y);
}

void InputManager::onMousePressed(int button)
{
    this->_mouseButtonStates[button] = true;
}

void InputManager::onMouseReleased(int button)
{
    this->_mouseButtonStates[button] = false;
}

void InputManager::endFrame()
{
    this->_prevKeyStates = this->_keyStates;
    this->_prevMouseButtonStates = this->_mouseButtonStates;
    this->_prevMousePos = this->_mousePos;
}
