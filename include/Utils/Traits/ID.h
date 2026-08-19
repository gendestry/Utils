#pragma once
#include <cstdint>

namespace Utils::Traits
{

/*
IDs are uniquely generated withing the extended class
Usage:
struct Foo : IDGenerator<Foo>
{};
*/
template <typename T> class IDGenerator
{
  public:
    IDGenerator() : m_id(id_generator++) {}
    uint64_t getUID() const { return m_id; }

  private:
    static uint64_t id_generator;
    uint64_t m_id;
};

template <typename T> uint64_t IDGenerator<T>::id_generator = 0;

/*
All IDs are uniquely generated, across all classes
*/
class UniqueIDGenerator
{
  public:
    UniqueIDGenerator() : uid(id_generator++) {}
    uint64_t getUID() const { return uid; }

  private:
    uint64_t uid = 0;
    static uint64_t id_generator;
};

uint64_t UniqueIDGenerator::id_generator = 0;
} // namespace Utils::Traits