//
// Created by bobi on 30. 03. 26.
//

#pragma once
#include <string>

namespace Utils::Traits {
    struct Stringify {
        virtual ~Stringify() = default;
        virtual std::string toString() = 0;
    };
}