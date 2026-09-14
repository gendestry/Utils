#include "Utils/Math/Interval.h"

namespace Utils::Maths
{
void Interval::fragment()
{
    auto bit = m_bases.begin();

    if (bit != m_bases.end())
    {
        auto cit = std::next(bit);

        while (cit != m_bases.end())
        {
            if (bit->touches(*cit))
            {
                bit->merge(*cit);
                cit = m_bases.erase(cit);
            }
            else
            {
                bit = cit;
                cit = std::next(cit);
            }
        }
    }

    cache();
}

void Interval::cache()
{
    m_cache.clear();

    uint64_t total = 0;
    for (const auto &base : m_bases)
        total += base.getSize();

    m_cache.reserve(total);

    // m_bases is kept sorted and non-overlapping, so appending each base's
    // values in order yields a sorted, duplicate-free cache.
    for (const auto &base : m_bases)
        m_cache.insert(m_cache.end(), base.values().begin(), base.values().end());
}

void Interval::add(IntervalBase base)
{
    if (m_bases.empty())
    {
        m_bases.emplace_back(std::move(base));
        cache();
        return;
    }

    auto it = m_bases.begin();
    while (it != m_bases.end() && *it < base)
        ++it;

    m_bases.insert(it, std::move(base));
    fragment();
};

void Interval::remove(const IntervalBase &removal)
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

    // cache(), not fragment(): removal only shrinks or splits bases, so it
    // can never leave two of them touching. There is nothing to re-merge.
    cache();
}

bool Interval::contains(uint64_t value) const
{
    for (const auto &it : m_bases)
    {
        if (it.contains(value))
        {
            return true;
        }
        // m_bases is sorted, so once a base starts past the value we are done.
        if (it > value)
        {
            break;
        }
    }
    return false;
}

Interval &Interval::operator+=(uint64_t value)
{
    add(value);
    return *this;
}

Interval &Interval::operator-=(uint64_t value)
{
    remove(value);
    return *this;
}

Interval &Interval::operator|=(const Interval &other)
{
    // Self-union is a no-op. Guarded up front because add() -> fragment()
    // erases from m_bases, which would invalidate the loop's iterator into
    // other.m_bases when the two are the same list.
    if (this == &other)
        return *this;

    for (const auto &base : other.m_bases)
        add(base);

    return *this;
}

Interval &Interval::operator&=(const Interval &other)
{
    // operator& builds a fresh result before assigning, so self-intersection
    // is safe without a guard.
    *this = *this & other;
    return *this;
}

Interval &Interval::operator-=(const Interval &other)
{
    // Removing a set from itself leaves nothing. Same aliasing hazard as |=:
    // remove() erases from m_bases while we walk other.m_bases.
    if (this == &other)
    {
        clear();
        return *this;
    }

    for (const auto &base : other.m_bases)
        remove(base);

    return *this;
}

Interval operator|(const Interval &lhs, const Interval &rhs)
{
    Interval result = lhs;

    for (const auto &base : rhs.m_bases)
        result.add(base);

    return result;
}

Interval operator&(const Interval &lhs, const Interval &rhs)
{
    Interval result;

    for (const auto &a : lhs.m_bases)
    {
        for (const auto &b : rhs.m_bases)
        {
            if (!a.overlaps(b))
                continue;

            result.add(std::max(a.getFrom(), b.getFrom()), std::min(a.getTo(), b.getTo()));
        }
    }

    return result;
}

Interval operator-(const Interval &lhs, const Interval &rhs)
{
    // Self-safe by construction: result is a copy, so `a - a` never compares
    // equal in operator-= and just removes everything the ordinary way.
    Interval result = lhs;
    result -= rhs;
    return result;
}
}
