#pragma once

#include "../EventTypes.hpp"
#include "../../../Core/Entity/Entity.hpp"

struct SelectionEvent : public Event
{
    EntityID entityID;
    bool selected;

    SelectionEvent(EntityID id, bool sel)
        : Event(EventType::SELECTION), entityID(id), selected(sel) {}
};
