#pragma once
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace Utils::Commands
{
class Args
{
    std::vector<std::string> m_tokens;

  public:
    Args() = default;
    explicit Args(const std::string &line);

    size_t size() const;
    bool empty() const;

    const std::string &command() const;
    const std::string &operator[](size_t i) const;

    bool has(size_t i) const;

    bool isInt(size_t i) const;
    std::optional<int> getInt(size_t i) const;

    std::optional<std::string> value(std::initializer_list<std::string_view> keys) const;
    std::optional<std::string> value(std::string_view key) const;

    const std::vector<std::string> &raw() const;
    auto begin() const { return m_tokens.begin(); }
    auto end() const { return m_tokens.end(); }
};
} // namespace Utils::Commands