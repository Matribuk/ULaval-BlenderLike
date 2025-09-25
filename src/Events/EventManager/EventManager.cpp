#include "EventManager.hpp"

void EventManager::processEvents()
{
    for (auto& e : this->_eventQueue) {
        auto it = this->_subscribers.find(typeid(*e).hash_code());
        if (it != this->_subscribers.end()) {
            for (auto& callback : it->second) {
                callback(*e);
            }
        }
    }
    this->_eventQueue.clear();
}
