#pragma once
#include <vector>
#include <string>
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

    struct MatchInfo
    {
        bool matched = false;
        unsigned int current = 0;
        std::string match;
        std::string fullmatch;
        std::vector<MatchInfo> groups;
    };

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

        virtual std::string toString() = 0;
        virtual std::string toPrettyString() = 0;
        virtual unsigned int _match(std::string text, unsigned int start, bool ignoreAllMathced = false) = 0;
        virtual MatchInfo _matchS(std::string text, unsigned int start, bool ignoreAllMathced = false) = 0;
        virtual Match match(std::string text, unsigned int start, bool ignoreAllMathced = false) = 0;
        virtual MatchInfo _match_info(std::string text, unsigned int start, bool ignoreAllMathced = false) { return MatchInfo{};};
        virtual MatchInfo match_info(std::string text, unsigned int start, bool ignoreAllMathced = false) { return MatchInfo{};};

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