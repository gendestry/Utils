//
// Created by bobi on 30. 03. 26.
//

#pragma once
#include "Utils/Text/String.h"
#include <string>

namespace Utils::Traits
{
struct Stringify
{
    virtual ~Stringify() = default;
    virtual std::string toString() const = 0;
};
} // namespace Utils::Traits