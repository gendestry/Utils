#include "Utils/Regex/Engine/AST/AST.h"
#include "Utils/Colors/Font.h"
#include "Utils/Text/Stream.h"
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
};