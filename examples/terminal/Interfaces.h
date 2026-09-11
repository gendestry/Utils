//
// Created by bobi on 11. 9. 26.
//

#pragma once
#include <functional>
#include "KeyEvent.h"

struct OnEvent
{
    using Callback = std::function<void(Event&)>;
    Callback callback;

    virtual ~OnEvent() = default;

    void setCallback(Callback cb) {callback = std::move(cb);}
    virtual void onEvent(Event& event)
    {
        if (callback)
            callback(event);
    }
};
