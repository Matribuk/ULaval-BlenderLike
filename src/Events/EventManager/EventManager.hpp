#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "../EventTypes/EventTypes.hpp"

class EventManager {
    public:
        EventManager() = default;
        ~EventManager() = default;

        template <typename T>
        void subscribe(std::function<void(const T&)> callback) {
            auto& subs = this->_subscribers[typeid(T).hash_code()];
            subs.push_back([callback](const Event& e) {
                callback(static_cast<const T&>(e));
            });
        }

        template <typename T>
        void emit(const T& event) {
            this->_eventQueue.push_back(std::make_shared<T>(event));
        }

        void processEvents();

    private:
        std::unordered_map<size_t, std::vector<std::function<void(const Event&)>>> _subscribers;
        std::vector<std::shared_ptr<Event>> _eventQueue;
};
