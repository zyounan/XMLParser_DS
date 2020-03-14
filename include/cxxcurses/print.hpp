#pragma once
#include <utility>
#include "print/glyph_string.hpp"
#include "utility.hpp"
namespace cxxcurses {
// TODO: WINDOW*(cxxcurses::window) versions
inline void print(const int y, const int x,
                  const glyph_string& format) noexcept {
    format.print(y, x);
}
inline void print(const int y, const glyph_string& format) noexcept {
    const auto x_centered{(getmaxx(stdscr) - format.size()) / 2};
    format.print(y, x_centered);
}
inline void print(const std::string& format) noexcept {
    const auto fuck = get_current_yx();
    auto y = fuck.first, x = fuck.second;
    mvwprintw(stdscr, y, x, format.c_str());
}
inline void print(const glyph_string& format) noexcept {
    const auto fuck = get_current_yx();
    auto y = fuck.first, x = fuck.second;
    format.print(y, x);
}
void print(const char c) { print(std::string(1, c)); }
// move and print versions
template <typename T, typename... Args>
constexpr void print(const int y, const int x, const glyph_string& format_str,
                     const T& arg, Args&&... args) noexcept {
    print(y, x, parse(format_str, arg), (std::forward<Args>(args))...);
}
struct printer {
    using coords_t = std::pair<int, int>;
    coords_t coords;
    template <typename... Args>
    constexpr void operator()(const std::string& format_str, Args&&... args) {
        const auto x = [&]() -> int {
            if (coords.second != -1)
                return coords.second;
            else
                return (getmaxx(stdscr) - format_str.size()) >> 1;
        }();
        print(coords.first, x, glyph_string{format_str},
              (std::forward<Args>(args))...);
    }
    void operator()(const char c) { this->operator()(std::string(1, c)); }
};
constexpr printer print(const int y, const int x) {
    return {std::make_pair(y, x)};
}
// centered
constexpr printer print(const int y) { return {std::make_pair(y, -1)}; }
// print at current cursor position versions

template <typename T, typename... Args>
constexpr void print(const glyph_string& format_str, const T& arg,
                     Args&&... args) noexcept {
    print(parse(format_str, arg), (std::forward<Args>(args))...);
}
template <typename... Args>
constexpr void print(const std::string& format_str, Args&&... args) noexcept {
    print(glyph_string{format_str}, (std::forward<Args>(args))...);
}
}  // namespace cxxcurses
// #endif  // CXXCURSES_PRINT_HPP
