#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace Utils::Terminal
{
class History
{
  private:
    std::vector<std::string> m_history;
    std::ptrdiff_t m_index = -1;
    std::string m_draft;

  public:
    void push(const std::string &input);

    std::optional<std::string> up(const std::string &input);

    std::optional<std::string> down();

    std::optional<std::string> find(const std::string &input);

    void reset();

    bool empty() const { return m_history.empty(); }

    std::size_t size() const { return m_history.size(); }
};
} // namespace Utils::Terminal