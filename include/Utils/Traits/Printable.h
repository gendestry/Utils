#pragma once

namespace Utils::Traits
{
struct Printable
{
    virtual ~Printable() = default;
    virtual void print() const {};
};
} // namespace Utils::Traits