#pragma once

#include "Events/EventTypes.hpp"

#include "Core/Entity.hpp"

struct SelectionEvent : public Event
{
    EntityID entityID;
    bool selected;

    SelectionEvent(EntityID id, bool sel)
        : Event(EventType::SELECTION), entityID(id), selected(sel) {}
};
