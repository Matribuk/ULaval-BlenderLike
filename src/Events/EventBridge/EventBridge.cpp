#include "EventBridge.hpp"

EventBridge::EventBridge(EventManager& eventManager) : _eventManager(eventManager) {}

void EventBridge::onKeyPressed(int key)
{
    this->_eventManager.emit(KeyEvent(key, KeyEventType::Pressed));
}

void EventBridge::onKeyReleased(int key)
{
    this->_eventManager.emit(KeyEvent(key, KeyEventType::Released));
}

void EventBridge::onMousePressed(int x, int y, int button)
{
    this->_eventManager.emit(MouseEvent(x, y, button, MouseEventType::Pressed));
}

void EventBridge::onMouseReleased(int x, int y, int button)
{
    this->_eventManager.emit(MouseEvent(x, y, button, MouseEventType::Released));
}

void EventBridge::onMouseMoved(int x, int y)
{
    this->_eventManager.emit(MouseEvent(x, y, -1, MouseEventType::Moved));
}

void EventBridge::onMouseDragged(int x, int y, int button)
{
    this->_eventManager.emit(MouseEvent(x, y, button, MouseEventType::Dragged));
}

void EventBridge::onMouseScrolled(int x, int y, float sx, float sy)
{
    this->_eventManager.emit(MouseEvent(x, y, -1, MouseEventType::Scrolled));
}
