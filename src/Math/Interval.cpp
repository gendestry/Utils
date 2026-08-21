#include "Utils/Math/Interval.h"

namespace Utils::Maths
{

void Interval::fragment()
{
    auto bit = m_bases.begin();

    if (bit == m_bases.end())
        return;

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

// bool IntervalBase::contains(uint64_t value) const { return value >= start && value <= end; }

// bool IntervalBase::overlaps(const IntervalBase &other) const
// {
//     return start <= other.end && other.start <= end;
// }

// // Also merge directly adjacent intervals:
// // [0,10] + [11,20]
// bool IntervalBase::touches(const IntervalBase &other) const
// {
//     return start <= other.end + 1 && other.start <= end + 1;
// }

// void IntervalBase::merge(const IntervalBase &other)
// {
//     start = std::min(start, other.start);
//     end = std::max(end, other.end);
//     fill();
// }

// bool IntervalBase::operator<(const IntervalBase &other) const { return end < other.start; }
// bool IntervalBase::operator<(uint64_t value) const { return end < value; }

// bool IntervalBase::operator>(const IntervalBase &other) const { return start > other.end; }
// bool IntervalBase::operator>(uint64_t value) const { return start > value; }

// const std::set<uint64_t> &IntervalBase::values() const { return cache; }

} // namespace Utils::Maths

// std::string IntervalBase::toString() const override
// {
//     return Utils::String::format("[{}...{}]", start, end);
// }