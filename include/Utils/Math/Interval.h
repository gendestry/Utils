#pragma once
#include "Utils/Text/Stream.h"
#include "Utils/Traits/Stringify.h"
#include <algorithm>
#include <concepts>
#include <cstdint>
#include <iterator>
#include <list>
#include <ranges>
#include <set>
#include <vector>
namespace Utils::Maths
{
class IntervalBase : public Traits::Stringify
{
    uint64_t start = 0;
    uint64_t end = 0;
    std::set<uint64_t> cache;

    void fill()
    {
        cache.clear();
        auto r = std::views::iota(start, end + 1);
        cache.insert(r.begin(), r.end());
    }

  public:
    IntervalBase(uint64_t from, uint64_t to) : start(from), end(to) { fill(); }

    uint64_t getFrom() const { return start; }
    uint64_t getTo() const { return end; }

    uint64_t getSize() const { return end - start + 1; }

    bool contains(uint64_t value) const { return value >= start && value <= end; }

    bool overlaps(const IntervalBase &other) const
    {
        return start <= other.end && other.start <= end;
    }

    // Also merge directly adjacent intervals:
    // [0,10] + [11,20]
    bool touches(const IntervalBase &other) const
    {
        return start <= other.end + 1 && other.start <= end + 1;
    }

    void merge(const IntervalBase &other)
    {
        start = std::min(start, other.start);
        end = std::max(end, other.end);
        fill();
    }

    bool operator<(const IntervalBase &other) const { return end < other.start; }
    bool operator<(uint64_t value) const { return end < value; }

    bool operator>(const IntervalBase &other) const { return start > other.end; }
    bool operator>(uint64_t value) const { return start > value; }

    [[nodiscard]] const std::set<uint64_t> &values() const { return cache; }

    [[nodiscard]] std::string toString() const override
    {
        if (start == end)
        {
            return Utils::String::format("{}", end);
        }
        return Utils::String::format("{}...{}", start, end);
    }
};

class Interval : public Traits::Stringify
{

    std::list<IntervalBase> m_bases;
    std::vector<uint64_t> m_cache;

    void fragment();
    void cache();

  public:
    // void add(uint64_t from, uint64_t to) { Int m_bases.emplace_back(from, to); }
    void add(IntervalBase base);
    void add(uint64_t from, uint64_t to) { add(IntervalBase(from, to)); }
    void add(uint64_t value) { add(IntervalBase{value, value}); }

    // Accepts any range of integrals (vector<uint16_t>, span, initializer_list, ...)
    template <std::ranges::input_range R>
        requires std::integral<std::ranges::range_value_t<R>>
    void add(const R &values)
    {
        for (const auto &v : values)
        {
            add(static_cast<uint64_t>(v));
        }
    };

    void remove(const IntervalBase &removal);
    void remove(uint64_t from, uint64_t to) { remove(IntervalBase(from, to)); }
    void remove(uint64_t value) { remove(IntervalBase(value, value)); }

    template <std::ranges::input_range R>
        requires std::integral<std::ranges::range_value_t<R>>
    void remove(const R &values)
    {
        for (const auto &v : values)
        {
            remove(static_cast<uint64_t>(v));
        }
    };

    bool empty() const { return m_bases.empty(); }

    void clear()
    {
        m_bases.clear();
        m_cache.clear();
    }

    [[nodiscard]] bool contains(uint64_t value) const;

    [[nodiscard]] const std::vector<uint64_t> &values() const { return m_cache; }

    [[nodiscard]] uint64_t size() const { return m_cache.size(); }

    friend Interval operator|(const Interval &lhs, const Interval &rhs);
    friend Interval operator&(const Interval &lhs, const Interval &rhs);

    Interval &operator+=(uint64_t value);
    Interval &operator-=(uint64_t value);

    // Set algebra against another Interval. Each guards against self-aliasing:
    // walking other.m_bases while add()/remove() erase from that same list
    // would invalidate the iterator.
    Interval &operator|=(const Interval &other);
    Interval &operator&=(const Interval &other);
    Interval &operator-=(const Interval &other);

    [[nodiscard]] std::string toString() const override
    {
        Utils::Text::Stream s;
        s << "[";

        bool first = true;
        for (const auto &base : m_bases)
        {
            if (!first)
                s << ",";

            s << base.toString();
            first = false;
        }

        s << "]";
        return s.end();
    }
};

Interval operator|(const Interval &lhs, const Interval &rhs);
Interval operator&(const Interval &lhs, const Interval &rhs);
// Not a friend: it goes through operator-=, so it needs no private access.
Interval operator-(const Interval &lhs, const Interval &rhs);
} // namespace Utils::Maths
