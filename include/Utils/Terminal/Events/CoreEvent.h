//
// Created by bobi on 19. 9. 26.
//

#pragma once
#include "Event.h"

namespace Utils::Terminal::Events
{
struct CoreEvent : Event
{
    EVENT_CLASS_CATEGORY(EventCategoryApplication);
};

struct Update : CoreEvent
{
    EVENT_CLASS_TYPE(UPDATE);
};
}