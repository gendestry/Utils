//
// Created by bobi on 11. 9. 26.
//

#pragma once
#include <functional>
#include "Utils/Terminal/Events/KeyEvent.h"

namespace Utils::Terminal::Iface
{
struct OnEvent
{
    using Callback = std::function<void(Events::Event&)>;
    Callback callback;

    virtual ~OnEvent() = default;

    void setCallback(Callback cb) {callback = std::move(cb);}
    virtual void onEvent(Events::Event& event)
    {
        if (callback)
            callback(event);
    }
};
}