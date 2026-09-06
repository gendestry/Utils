#include "Utils/Regex/Engine/AST/AST.h"
#include "Utils/Colors/Font.h"
#include "Utils/Text/Stream.h"

#include <algorithm>
#include <climits>
#include <functional>
#include <iostream>

using namespace Utils;

namespace Utils::Regex::Engine {
Match AstNodeOps::match(std::string text, unsigned int start, bool ignoreAllMathced) {

    unsigned int s = start;
    unsigned int m = _match(text, start, ignoreAllMathced);

    switch (m_OpType) {
        case NONE:
            return {m != s, m};
        case PLUS:
            if (m != s)
            {
                s = m;
                m = _match(text, s, ignoreAllMathced);

                while (m != s)
                {
                    s = m;
                    m = _match(text, s, ignoreAllMathced);
                }

                return {true, m};
            }

            return {false, s};
        case ASTERIX:
            while (m != s)
            {
                s = m;
                m = _match(text, s, ignoreAllMathced);
            }

            return {true, m};
        case QUESTION_MARK:
            return {true, m};
        case RANGE:
            auto r1 = m_Range.start;
            auto r2 = m_Range.end;
            int i = 0;
            // min ammount
            for (; i < r1; i++) {
                if (m != s) {
                    s = m;
                    m = _match(text, s, ignoreAllMathced);
                }
                else {
                    return {false, s};
                }
            }

            while (m != s)
            {
                s = m;
                m = _match(text, s, ignoreAllMathced);
                i++;
            }

            if (i <= r2)
            {
                return {true, m};
            }

            return {false, s};
    }


    return {false, s};
}



std::optional<MatchInfo> AstNodeOps::match_info(std::string text, unsigned int start, bool ignoreAllMathced)
{
    if (start >= text.size())
        return {};

    unsigned int s = start;
    MatchInfo mi = _match_info(text, start, ignoreAllMathced);
    unsigned int m = mi.start;

    MatchInfo tm = mi;


    switch (m_OpType)
    {
    case NONE:
            if (mi.start == start) {
                return {};
            }
            return mi;
        // return {m != s, m};
    case PLUS:
        if (m != s)
        {
            s = m;
            tm = _match_info(text, s, ignoreAllMathced);
            m = tm.start;
            mi.match += tm.match;
            mi.groups.insert(mi.groups.end(), tm.groups.begin(), tm.groups.end());

            while (m != s)
            {
                s = m;
                tm = _match_info(text, s, ignoreAllMathced);
                m = tm.start;
                mi.match += tm.match;
                mi.groups.insert(mi.groups.end(), tm.groups.begin(), tm.groups.end());
            }

            mi.start = m;
            return mi;
        }

        return {};
    case ASTERIX:
        while (m != s)
        {
            s = m;
            tm = _match_info(text, s, ignoreAllMathced);
            m = tm.start;
            mi.match += tm.match;
            mi.groups.insert(mi.groups.end(), tm.groups.begin(), tm.groups.end());
        }

        mi.start = m;
        return mi;
    case QUESTION_MARK:
        return mi;
    case RANGE:
        auto r1 = m_Range.start;
        auto r2 = m_Range.end;
        int i = 0;
        // min ammount
        for (; i < r1; i++) {
            if (m != s) {
                s = m;
                tm = _match_info(text, s, ignoreAllMathced);
                m = tm.start;
                mi.match += tm.match;
                mi.groups.insert(mi.groups.end(), tm.groups.begin(), tm.groups.end());
            }
            else {
                return {};
            }
        }

        while (m != s)
        {
            s = m;
            tm = _match_info(text, s, ignoreAllMathced);
            m = tm.start;
            mi.match += tm.match;
            mi.groups.insert(mi.groups.end(), tm.groups.begin(), tm.groups.end());
            i++;
        }

        if (i <= r2)
        {
            mi.start = m;
            return mi;
        }

        return {};
    }

    return {};
}

std::vector<MatchInfo> AstNodeOps::applyRepetition(unsigned int start,
    const std::function<std::vector<MatchInfo>(unsigned int)> &oneIteration) const
{
    std::vector<MatchInfo> results;

    if (m_OpType == NONE)
        return oneIteration(start);

    if (m_OpType == QUESTION_MARK)
    {
        MatchInfo empty;
        empty.start = start;

        // Lazy: try skipping the node before trying to match it.
        if (m_Lazy)
            results.push_back(empty);

        for (auto &candidate : oneIteration(start))
            results.push_back(candidate);

        if (!m_Lazy)
            results.push_back(empty);

        return results;
    }

    unsigned int minReps = (m_OpType == PLUS) ? 1 : (m_OpType == RANGE ? m_Range.start : 0);
    unsigned int maxReps = (m_OpType == RANGE) ? m_Range.end : UINT_MAX;

    std::function<void(unsigned int, unsigned int, MatchInfo)> rec =
        [&](unsigned int reps, unsigned int pos, MatchInfo acc)
    {
        if (reps >= minReps)
            results.push_back(acc);

        if (reps >= maxReps)
            return;

        for (auto &candidate : oneIteration(pos))
        {
            if (candidate.start == pos)   // no progress -> can't repeat further
                continue;

            MatchInfo next = acc;
            next.match += candidate.match;
            next.groups.insert(next.groups.end(), candidate.groups.begin(), candidate.groups.end());
            next.start = candidate.start;
            rec(reps + 1, candidate.start, next);
        }
    };

    MatchInfo seed;
    seed.start = start;
    rec(0, start, seed);

    // rec() collects fewest-iterations-first; greedy wants the opposite order.
    if (!m_Lazy)
        std::reverse(results.begin(), results.end());

    return results;
}

std::vector<MatchInfo> AstNodeOps::match_info_candidates(std::string text, unsigned int start, bool ignoreAllMatched)
{
    return applyRepetition(start, [&](unsigned int pos) -> std::vector<MatchInfo>
    {
        if (pos >= text.size())
            return {};

        MatchInfo mi = _match_info(text, pos, ignoreAllMatched);
        if (mi.start == pos)
            return {};

        return { mi };
    });
}

std::vector<MatchInfo> AstNodeOps::matchSequenceCandidates(const std::vector<AstNodeOps*> &ops, const std::string &text, unsigned int start, bool ignoreAllMatched)
{
    std::vector<MatchInfo> results;

    std::function<void(std::size_t, unsigned int, MatchInfo)> rec = [&](std::size_t opIndex, unsigned int pos, MatchInfo acc)
    {
        if (opIndex == ops.size())
        {
            acc.start = pos;
            results.push_back(acc);
            return;
        }

        AstNodeOps *op = ops[opIndex];
        for (auto &candidate : op->match_info_candidates(text, pos, ignoreAllMatched))
        {
            MatchInfo next = acc;
            if (!op->shouldIgnore())
                next.match += candidate.match;
            collectGroups(next, *op, candidate);
            rec(opIndex + 1, candidate.start, next);
        }
    };

    MatchInfo seed;
    seed.start = start;
    rec(0, start, seed);

    return results;
}

};