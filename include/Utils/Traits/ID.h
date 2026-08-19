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
    IDGenerator(const IDGenerator &other) : m_id(id_generator++) {}
    IDGenerator(IDGenerator &&other) : m_id(other.m_id) {}

    IDGenerator &operator=(const IDGenerator &other) { m_id = id_generator++; }
    IDGenerator &operator=(IDGenerator &&other) { m_id = other.m_id; }

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
    UniqueIDGenerator() : m_id(id_generator++) {}
    UniqueIDGenerator(const UniqueIDGenerator &other) : m_id(id_generator++) {}
    UniqueIDGenerator(UniqueIDGenerator &&other) : m_id(other.m_id) {}

    UniqueIDGenerator &operator=(const UniqueIDGenerator &other) { m_id = id_generator++; }
    UniqueIDGenerator &operator=(UniqueIDGenerator &&other) { m_id = other.m_id; }

    uint64_t getUID() const { return m_id; }

  private:
    static uint64_t id_generator;
    uint64_t m_id = 0;
};

} // namespace Utils::Traits