#include "EventBridge.hpp"

EventBridge::EventBridge(EventManager& eventManager) : _eventManager(eventManager) {}

EventBridge::~EventBridge()
{
    remove();
}

void EventBridge::setup() {
    ofAddListener(ofEvents().keyPressed, this, &EventBridge::onKeyPressed);
    ofAddListener(ofEvents().keyReleased, this, &EventBridge::onKeyReleased);
    ofAddListener(ofEvents().mouseMoved, this, &EventBridge::onMouseMoved);
    ofAddListener(ofEvents().mousePressed, this, &EventBridge::onMousePressed);
    ofAddListener(ofEvents().mouseReleased, this, &EventBridge::onMouseReleased);
}

void EventBridge::remove() {
    ofRemoveListener(ofEvents().keyPressed, this, &EventBridge::onKeyPressed);
    ofRemoveListener(ofEvents().keyReleased, this, &EventBridge::onKeyReleased);
    ofRemoveListener(ofEvents().mouseMoved, this, &EventBridge::onMouseMoved);
    ofRemoveListener(ofEvents().mousePressed, this, &EventBridge::onMousePressed);
    ofRemoveListener(ofEvents().mouseReleased, this, &EventBridge::onMouseReleased);
}

void EventBridge::onKeyPressed(ofKeyEventArgs & args)
{
    this->_eventManager.emit(KeyEvent(args.key, KeyEventType::Pressed));
}

void EventBridge::onKeyReleased(ofKeyEventArgs & args)
{
    this->_eventManager.emit(KeyEvent(args.key, KeyEventType::Released));
}

void EventBridge::onMousePressed(ofMouseEventArgs & args)
{
    this->_eventManager.emit(MouseEvent(args.x, args.y, args.button, MouseEventType::Pressed));
}

void EventBridge::onMouseReleased(ofMouseEventArgs & args)
{
    this->_eventManager.emit(MouseEvent(args.x, args.y, args.button, MouseEventType::Released));
}

void EventBridge::onMouseMoved(ofMouseEventArgs & args)
{
    this->_eventManager.emit(MouseEvent(args.x, args.y, -1, MouseEventType::Moved));
}

void EventBridge::onMouseDragged(ofMouseEventArgs & args)
{
    this->_eventManager.emit(MouseEvent(args.x, args.y, args.button, MouseEventType::Dragged));
}

void EventBridge::onMouseScrolled(ofMouseEventArgs & args)
{
    this->_eventManager.emit(MouseEvent(args.x, args.y, -1, MouseEventType::Scrolled));
}
