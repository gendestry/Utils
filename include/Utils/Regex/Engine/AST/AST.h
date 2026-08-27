#pragma once
#include <vector>
#include <string>

#include "Utils/Text/Stream.h"
#include "Utils/Text/String.h"

namespace Utils::Regex::Engine
{
    struct Match
    {
        bool matched = false;
        unsigned int current = 0;
        // std::string match;
        // std::string fullmatch;
        //
        // std::vector<Match> groups;
    };

    struct MatchInfo {
        unsigned int start = 0;
        unsigned int len = 0;
        std::string match;
        std::string fullmatch;

        std::vector<MatchInfo> groups;

        MatchInfo(){}
        MatchInfo(unsigned int start, std::string match)
            : start(start), match(match), len(match.length()) {}
        MatchInfo(unsigned int start, std::string match, std::string fullmatch)
            : start(start), match(match), fullmatch(fullmatch), len(match.length()) {}

        unsigned int end() const
        {
            return len == 0 ? start : start + len - 1;
        }
        std::string toString() const {
            Utils::Text::Stream s;
            // s << match << "\n[";
            s << "[" << match;
            if (!groups.empty())
            {
                s << " ";
                for(const auto& group : groups) {
                    s << group.toString() << "";
                }
            }

            s << "]";
            s << std::format("(s:{},e:{})", start, end());
            return s.end();
        }
    };

    // struct MatchInfo
    // {
    //     bool matched = false;
    //     unsigned int current = 0;
    //     std::string match;
    //     std::string fullmatch;
    //     std::vector<MatchInfo> groups;
    // };

    class AstNodeOps {
    protected:
        enum WhatDo {
            IGNORE,
            CAPTURE,
            DEFAULT
        } m_whatDo = DEFAULT;

        // enum NodeType {
        //     PAR
        // };
    public:
        enum OpType
        {
            PLUS,
            ASTERIX,
            QUESTION_MARK,
            RANGE,
            NONE
        } m_OpType = NONE;


        struct Location
        {
            int start;
            int end;
        } m_Location;

        struct Range {
            unsigned int start;
            unsigned int end;
        } m_Range;

        AstNodeOps(Location location) : m_Location(location) {}
        AstNodeOps(Location location, unsigned int r1, unsigned int r2)
            : m_Location(location), m_Range {r1, r2}
        {}

        std::string getmatch();

        bool shouldIgnore() const {
            return m_whatDo == IGNORE;
        }

        virtual bool shouldCapture() const {
            return m_whatDo == CAPTURE;
        }

        bool shouldDefault() const {
            return m_whatDo == DEFAULT;
        }

        // The one rule for building the capture tree: a {..} child becomes a nested
        // capture, and any other node (paren, enclosure) hands its own captures straight
        // up. That way only {..} ever appears in MatchInfo::groups, one entry per {..}
        // the pattern actually went through, in source order.
        static void collectGroups(MatchInfo &into, const AstNodeOps &child, const MatchInfo &info)
        {
            if (child.shouldIgnore())
                return;

            if (child.shouldCapture())
                into.groups.push_back(info);
            else
                into.groups.insert(into.groups.end(), info.groups.begin(), info.groups.end());
        }

        virtual std::string toString() = 0;
        virtual std::string toPrettyString() = 0;
        virtual unsigned int _match(std::string text, unsigned int start, bool ignoreAllMathced = false) = 0;
        virtual Match match(std::string text, unsigned int start, bool ignoreAllMathced = false);
        virtual MatchInfo _match_info(std::string text, unsigned int start, bool ignoreAllMathced = false) { return MatchInfo{};};
        virtual std::optional<MatchInfo> match_info(std::string text, unsigned int start, bool ignoreAllMathced = false);

        bool isOtional() const { if (m_OpType == QUESTION_MARK || m_OpType == ASTERIX) return true; return false; };

        std::string toOpString()
        {
            switch (m_OpType)
            {
            case PLUS:
                return "+";
            case ASTERIX:
                return "*";
            case QUESTION_MARK:
                return "?";
                case RANGE:
                return Utils::String::concat("{",m_Range.start, ",",m_Range.end, "}");
            default:
                return "";
            }
        }
    };














};