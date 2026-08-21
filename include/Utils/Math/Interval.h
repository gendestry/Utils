#pragma once
#include "Utils/Text/Stream.h"
#include "Utils/Traits/Stringify.h"
#include <algorithm>
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
    uint64_t m_size = 0;
    std::set<uint64_t> cache;

    void fill()
    {
        cache.clear();
        auto r = std::views::iota(start, end + 1);
        cache.insert(r.begin(), r.end());
    }

  public:
    IntervalBase(uint64_t from, uint64_t to) : start(from), end(to), m_size(to - from) { fill(); }

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
        return Utils::String::format("[{}...{}]", start, end);
    }
};

class Interval : public Traits::Stringify
{

    std::list<IntervalBase> m_bases;

    void fragment();

  public:
    // void add(uint64_t from, uint64_t to) { Int m_bases.emplace_back(from, to); }
    void add(IntervalBase base)
    {
        if (m_bases.empty())
        {
            m_bases.emplace_back(std::move(base));
            return;
        }

        auto it = m_bases.begin();
        while (it != m_bases.end() && *it < base)
            ++it;

        m_bases.insert(it, std::move(base));
        fragment();
    };

    void add(uint64_t from, uint64_t to) { add(IntervalBase(from, to)); }
    void add(uint64_t value) { add(IntervalBase{value, value}); }

    void add(std::vector<uint64_t> values)
    {
        for (auto &v : values)
        {
            add(v);
        }
    };

    void remove(const IntervalBase &removal)
    {
        auto it = m_bases.begin();

        while (it != m_bases.end())
        {
            // Existing interval is completely before removal.
            if (*it < removal)
            {
                ++it;
                continue;
            }

            // Existing interval is completely after removal.
            if (*it > removal)
                break;

            const auto from = it->getFrom();
            const auto to = it->getTo();

            // removal completely covers this interval.
            if (removal.getFrom() <= from && removal.getTo() >= to)
            {
                it = m_bases.erase(it);
                continue;
            }

            // Removal cuts off the left side.
            if (removal.getFrom() <= from)
            {
                *it = IntervalBase(removal.getTo() + 1, to);
                break;
            }

            // Removal cuts off the right side.
            if (removal.getTo() >= to)
            {
                *it = IntervalBase(from, removal.getFrom() - 1);
                ++it;
                continue;
            }

            // Removal is completely inside the existing interval.
            // Split:
            //
            // [from ........ to]
            //       [removal]
            //
            // -> [from..removal.from-1]
            //    [removal.to+1..to]
            auto right = IntervalBase(removal.getTo() + 1, to);

            *it = IntervalBase(from, removal.getFrom() - 1);

            m_bases.insert(std::next(it), std::move(right));

            break;
        }
    }

    void remove(uint64_t from, uint64_t to) { remove(IntervalBase(from, to)); }
    void remove(uint64_t value) { remove(IntervalBase(value, value)); }

    void remove(std::vector<uint64_t> values)
    {
        for (auto &v : values)
        {
            remove(v);
        }
    };

    bool empty() const { return m_bases.empty(); }

    void clear() { m_bases.clear(); }

    bool contains(uint64_t value)
    {
        for (auto &it : m_bases)
        {
            if (it.contains(value))
            {
                return true;
            }
            if (it > value)
            {
                break;
            }
        }
        return false;
    }

    [[nodiscard]] std::vector<uint64_t> values() const
    {
        std::vector<uint64_t> ret;

        for (const auto &base : m_bases)
        {
            ret.insert(ret.end(), base.values().begin(), base.values().end());
        }

        return ret;
    }

    friend Interval operator|(const Interval &lhs, const Interval &rhs);
    friend Interval operator&(const Interval &lhs, const Interval &rhs);

    Interval &operator|=(const Interval &other)
    {
        for (const auto &base : other.m_bases)
            add(base);

        return *this;
    }

    Interval &operator&=(const Interval &other)
    {
        *this = *this & other;
        return *this;
    }

    [[nodiscard]] std::string toString() const override
    {
        Utils::Text::Stream s;
        s << "[";

        bool first = true;
        for (const auto &base : m_bases)
        {
            if (!first)
                s << ", ";

            s << base.toString();
            first = false;
        }

        s << "]";
        return s.end();
    }
};

Interval operator|(const Interval &lhs, const Interval &rhs);
Interval operator&(const Interval &lhs, const Interval &rhs);
} // namespace Utils::Maths
