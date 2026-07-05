#include "Utils/Commands/Args.h"
#include "Utils/Text/String.h"

namespace Utils::Commands
{
Args::Args(const std::string &line)
    : m_tokens(Utils::String::split(::Utils::String::normalize_spaces(line), " "))
{
}

size_t Args::size() const { return m_tokens.size(); }
bool Args::empty() const { return m_tokens.empty(); }

const std::string &Args::command() const
{
    static const std::string none;
    return m_tokens.empty() ? none : m_tokens[0];
}

const std::string &Args::operator[](size_t i) const
{
    static const std::string none;
    return i < m_tokens.size() ? m_tokens[i] : none;
}

bool Args::has(size_t i) const { return i < m_tokens.size(); }

bool Args::isInt(size_t i) const
{
    return i < m_tokens.size() && ::Utils::String::isInt(m_tokens[i]);
}

std::optional<int> Args::getInt(size_t i) const
{
    if (!isInt(i))
        return std::nullopt;
    return std::stoi(m_tokens[i]);
}

// ── key/value access ────────────────────────────────────
// For "ssid <value> pass <value>"-style lines: returns the token after
// the first occurrence of `key`. nullopt if `key` is absent or has no
// following value. Matches against any of the given aliases.
std::optional<std::string> Args::value(std::initializer_list<std::string_view> keys) const
{
    for (size_t i = 0; i + 1 < m_tokens.size(); ++i)
        for (auto k : keys)
            if (m_tokens[i] == k)
                return m_tokens[i + 1];
    return std::nullopt;
}

std::optional<std::string> Args::value(std::string_view key) const { return value({key}); }

const std::vector<std::string> &Args::raw() const { return m_tokens; }

auto Args::begin() const { return m_tokens.begin(); }
auto Args::end() const { return m_tokens.end(); }
} // namespace Utils::Commands