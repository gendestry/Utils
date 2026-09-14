#pragma once
#include "Utils/Traits/Printable.h"
#include "Utils/Traits/Stringify.h"

namespace Utils::Traits
{
struct Describe : public Printable, public Stringify
{
    virtual ~Describe() = default;
};
} // namespace Utils::Traits