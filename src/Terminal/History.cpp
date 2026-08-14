#include "Utils/Terminal/History.h"

using namespace Utils::Terminal;

void History::push(const std::string &input)
{
    if (!input.empty())
    {
        m_history.push_back(input);
    }

    reset();
}

std::optional<std::string> History::up(const std::string &input)
{
    if (m_history.empty())
    {
        return std::nullopt;
    }

    if (m_index == -1)
    {
        m_draft = input;
        m_index = static_cast<std::ptrdiff_t>(m_history.size()) - 1;
    }
    else if (m_index > 0)
    {
        --m_index;
    }

    return m_history[m_index];
}

std::optional<std::string> History::down()
{
    if (m_index == -1)
    {
        return std::nullopt;
    }

    if (m_index + 1 < static_cast<std::ptrdiff_t>(m_history.size()))
    {
        ++m_index;
        return m_history[m_index];
    }

    m_index = -1;
    auto ret = m_draft;
    m_draft.clear();
    return ret;
}

std::optional<std::string> History::find(const std::string &input)
{
    if (input.size() == 0)
    {
        return std::nullopt;
    }

    for (auto it = m_history.rbegin(); it != m_history.rend(); ++it)
    {
        if (it->starts_with(input))
            return *it;
    }

    return std::nullopt;
}

void History::reset()
{
    m_index = -1;
    m_draft.clear();
}