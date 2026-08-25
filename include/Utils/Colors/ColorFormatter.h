#pragma once
#include "Utils/Colors/Font.h"
#include <format>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

namespace Utils::Font
{

class FormatExpr
{
  public:
    using Renderer = std::function<std::string(std::string_view parent_color)>;

    FormatExpr(Renderer renderer) : renderer_(std::move(renderer)) {}

    std::string render(std::string_view color = "") const { return renderer_(color); }

    // A finished expression is just a coloured string, so let it be used as one:
    //     std::string s = Theme::dim("test {}", 1);
    std::string str() const { return render(); }
    operator std::string() const { return render(); }

  private:
    Renderer renderer_;
};

inline FormatExpr text(std::string_view str)
{
    return FormatExpr{[str = std::string(str)](std::string_view) { return str; }};
}

namespace detail
{
// Anything string-like (or another expression) is nested content; everything else is a
// std::format argument for the placeholders in that content.
template <typename T>
constexpr bool isContent =
    std::is_same_v<std::decay_t<T>, FormatExpr> || std::is_convertible_v<T, std::string_view>;

template <typename... Args> std::vector<FormatExpr> collectChildren(Args &&...args)
{
    std::vector<FormatExpr> children;

    children.reserve(sizeof...(Args));

    auto add = [&]<typename T>(T &&arg)
    {
        using TDecayed = std::decay_t<T>;

        if constexpr (std::is_same_v<TDecayed, FormatExpr>)
        {
            children.emplace_back(std::forward<T>(arg));
        }
        else if constexpr (isContent<T>)
        {
            children.emplace_back(text(arg));
        }
    };

    (add(std::forward<Args>(args)), ...);

    return children;
}

// Type-erased "fill in the placeholders" step. Substitution is deferred to render time so
// that nesting still works, but the argument types are preserved (format specs keep working).
using Applier = std::function<std::string(const std::string &)>;

template <typename... Args> Applier collectApplier(Args &&...args)
{
    auto stored = std::tuple_cat(
        [&]
        {
            if constexpr (isContent<Args>)
            {
                return std::tuple<>{};
            }
            else
            {
                return std::tuple<std::decay_t<Args>>(std::forward<Args>(args));
            }
        }()...
    );

    if constexpr (std::tuple_size_v<decltype(stored)> == 0)
    {
        return {};
    }
    else
    {
        return [stored = std::move(stored)](const std::string &fmt) mutable
        { return std::apply([&](auto &...a) { return std::vformat(fmt, std::make_format_args(a...)); }, stored); };
    }
}

inline std::string applyArgs(const Applier &applier, std::string rendered)
{
    return applier ? applier(rendered) : rendered;
}
} // namespace detail

// Transparent container: contributes no colour of its own, so children render against
// whatever colour encloses the group (plain terminal default at the top level).
//     format(group("connected to ", Theme::lbl("{}"), " on port ", Theme::num("{}")), host, port);
template <typename... Args> FormatExpr group(Args &&...args)
{
    return FormatExpr{[children = detail::collectChildren(std::forward<Args>(args)...),
                       applier = detail::collectApplier(std::forward<Args>(args)...)](
                          std::string_view parent_color
                      )
    {
        std::string result;

        for (const auto &child : children)
        {
            result += child.render(parent_color);
        }

        return detail::applyArgs(applier, std::move(result));
    }};
}

template <typename... Args> FormatExpr color(std::string color_code, Args &&...args)
{
    auto children = detail::collectChildren(std::forward<Args>(args)...);
    auto applier = detail::collectApplier(std::forward<Args>(args)...);

    return FormatExpr{[color_code = std::move(color_code), children = std::move(children),
                       applier = std::move(applier)](std::string_view parent_color)
    {
        std::string result;

        result += color_code;

        for (const auto &child : children)
        {
            result += child.render(color_code);
        }

        // The last child restored us to color_code on its way out; we are about to override
        // that immediately, so drop it rather than emitting a sequence nothing renders under.
        if (!children.empty() && result.ends_with(color_code))
        {
            result.resize(result.size() - color_code.size());
        }

        if (!parent_color.empty())
        {
            result += parent_color;
        }
        else
        {
            result += Font::colorReset;
        }

        return detail::applyArgs(applier, std::move(result));
    }};
};

template <typename... Args> FormatExpr RED(Args &&...args)
{
    return color(Font::colorRed, std::forward<Args>(args)...);
}

template <typename... Args> FormatExpr GREEN(Args &&...args)
{
    return color(Font::colorGreen, std::forward<Args>(args)...);
}

template <typename... Args> FormatExpr BLUE(Args &&...args)
{
    return color(Font::colorBlue, std::forward<Args>(args)...);
}

template <typename... Args> FormatExpr FG(const Colors::RGB &rgb, Args &&...args)
{
    return color(colorByRGB(rgb), std::forward<Args>(args)...);
}

template <typename... Args> FormatExpr BG(const Colors::RGB &rgb, Args &&...args)
{
    return color(colorByRGB(rgb, false), std::forward<Args>(args)...);
}

// Binds an RGB colour up front, yielding a callable usable exactly like FG/BG:
//     inline const auto NUMBER = themed({255, 180, 0});
//     format(NUMBER("v={}"), 7);
inline auto themed(Colors::RGB rgb)
{
    return [rgb](auto &&...args) { return FG(rgb, std::forward<decltype(args)>(args)...); };
}

inline auto themedBg(Colors::RGB rgb)
{
    return [rgb](auto &&...args) { return BG(rgb, std::forward<decltype(args)>(args)...); };
}

template <typename... Args> FormatExpr B(Args &&...args)
{
    return color(Font::colorBold, std::forward<Args>(args)...);
}

template <typename... Args> FormatExpr IT(Args &&...args)
{
    return color(Font::colorItalic, std::forward<Args>(args)...);
}

template <typename... Args> std::string format(const FormatExpr &formatExpr, Args &&...args)
{
    std::string colorised = formatExpr.render();

    if constexpr (sizeof...(Args) == 0)
    {
        return colorised;
    }
    else
    {
        return std::vformat(colorised, std::make_format_args(args...));
    }
}

} // namespace Utils::Font

// Lets a fully-substituted expression drop straight into std::format/print:
//     std::println("{}", Theme::dim("test {}", 1));
template <> struct std::formatter<Utils::Font::FormatExpr> : std::formatter<std::string>
{
    auto format(const Utils::Font::FormatExpr &expr, std::format_context &ctx) const
    {
        return std::formatter<std::string>::format(expr.render(), ctx);
    }
};