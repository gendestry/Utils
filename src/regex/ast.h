#pragma once
#include <vector>
#include <string>

namespace Regex
{
    struct Match
    {
        bool matched = false;
        unsigned int current = 0;
    };

    class AstNodeOps
    {
    public:
        enum OpType
        {
            PLUS,
            ASTERIX,
            QUESTION_MARK,
            NONE
        } m_OpType = NONE;

        struct Location
        {
            int start;
            int end;
        } m_Location;

        AstNodeOps(Location location) : m_Location(location) {}

        virtual std::string toString() = 0;
        virtual std::string toPrettyString() = 0;
        virtual unsigned int _match(std::string text, unsigned int start) = 0;
        virtual Match match(std::string text, unsigned int start) = 0;

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
            default:
                return "";
            }
        }
    };

    class AstNodeParen : public AstNodeOps
    {
    public:
        std::vector<std::vector<AstNodeOps *>> m_Ops;

        AstNodeParen(Location loc, std::vector<std::vector<AstNodeOps *>> ops, OpType optype = OpType::NONE) : AstNodeOps(loc), m_Ops(ops)
        {
            m_OpType = optype;
        }

        ~AstNodeParen()
        {
            for (auto &op : m_Ops)
            {
                for (auto &o : op)
                {
                    delete o;
                }
            }
        }

        unsigned int _match(std::string text, unsigned int start) override;
        Match match(std::string text, unsigned int start) override;

        std::string toString() override;
        std::string toPrettyString() override;
    };

    class AstNodeEscape : public AstNodeOps
    {
    public:
        enum EscapeType
        {
            CHAR,
            BIG_CHAR,
            ANY_CHAR,
            DIGIT,
            NEWLINE
        } m_EscapeType;

        std::string toEscapeString()
        {
            switch (m_EscapeType)
            {
            case CHAR:
                return "\\c";
            case BIG_CHAR:
                return "\\C";
            case ANY_CHAR:
                return "\\T";
            case DIGIT:
                return "\\d";
            case NEWLINE:
                return "\\n";
            default:
                return "\\X";
            }
        }

        AstNodeEscape(Location loc, EscapeType escapeType, OpType optype = OpType::NONE) : AstNodeOps(loc), m_EscapeType(escapeType)
        {
            m_OpType = optype;
        }

        unsigned int _match(std::string text, unsigned int start) override;
        Match match(std::string text, unsigned int start) override;

        std::string toString() override;
        std::string toPrettyString() override;
    };

    class AstNodeTxt : public AstNodeOps
    {
    public:
        std::string txt;
        AstNodeTxt(Location loc, std::string txt, OpType optype = OpType::NONE) : AstNodeOps(loc), txt(txt)
        {
            m_OpType = optype;
        }

        unsigned int _match(std::string text, unsigned int start) override;
        Match match(std::string text, unsigned int start) override;

        std::string toString() override;
        std::string toPrettyString() override;
    };

    class AstNodeRange : public AstNodeOps
    {
    public:
        char m_Start, m_End;
        AstNodeRange(Location loc, char start, char end, OpType optype = OpType::NONE) : AstNodeOps(loc), m_Start(start), m_End(end)
        {
            m_OpType = optype;
        }

        unsigned int _match(std::string text, unsigned int start) override;
        Match match(std::string text, unsigned int start) override;

        std::string toString() override;
        std::string toPrettyString() override;
    };
};