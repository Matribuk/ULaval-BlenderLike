#include "Manager/InputManager/InputManager.hpp"


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

bool InputManager::_getKeyState(int key, const std::unordered_map<int, bool>& stateMap) const
{
    auto it = stateMap.find(key);
    return (it != stateMap.end() && it->second);
}

bool InputManager::_checkTransition(int key, bool expectNow, bool expectPrev) const
{
    bool now = this->_getKeyState(key, this->_keyStates);
    bool prev = this->_getKeyState(key, this->_prevKeyStates);
    return (now == expectNow && prev == expectPrev);
}

bool InputManager::wasKeyJustPressed(int key) const
{
    return this->_checkTransition(key, true, false);
}

bool InputManager::wasKeyJustReleased(int key) const
{
    return this->_checkTransition(key, false, true);
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

void InputManager::registerKeyAction(int key, std::function<void()> action)
{
    this->_keyActions[key] = action;
}

void InputManager::processShortcuts()
{
    for (auto& [keys, action] : this->_shortcuts) {
        bool allPressed = true;
        for (int key : keys) {
            auto it = this->_keyStates.find(key);
            if (it == this->_keyStates.end() || !it->second) {
                allPressed = false;
                break;
            }
        }
        if (allPressed)
        action();
    }
}

void InputManager::processKeyActions()
{
    for (auto& [key, action] : this->_keyActions) {
        if (wasKeyJustPressed(key)) {
            action();
        }
    }
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

void InputManager::subscribeToEvents(EventManager& eventManager)
{
    eventManager.subscribe<KeyEvent>([this](const KeyEvent& e) {
        if (e.type == KeyEventType::Pressed)
            this->onKeyPressed(e.key);
        else
            this->onKeyReleased(e.key);
    });

    eventManager.subscribe<MouseEvent>([this](const MouseEvent& e) {
        switch(e.type) {
            case MouseEventType::Pressed:
                this->onMousePressed(e.button);
                break;
            case MouseEventType::Released:
                this->onMouseReleased(e.button);
                break;
            case MouseEventType::Moved:
            case MouseEventType::Dragged:
                this->onMouseMoved(e.x, e.y);
                break;
            default:
                break;
        }
    });
}

void InputManager::endFrame()
{
    this->_prevKeyStates = this->_keyStates;
    this->_prevMouseButtonStates = this->_mouseButtonStates;
    this->_prevMousePos = this->_mousePos;
}
