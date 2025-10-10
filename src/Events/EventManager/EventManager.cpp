#include "Events/EventManager/EventManager.hpp"

void EventManager::processEvents()
{
    auto eventsToProcess = std::move(this->_eventQueue);
    this->_eventQueue.clear();

    for (auto& e : eventsToProcess) {
        auto it = this->_subscribers.find(typeid(*e).hash_code());
        if (it != this->_subscribers.end()) {
            for (auto& callback : it->second) {
                callback(*e);
            }
        }
    }
}
