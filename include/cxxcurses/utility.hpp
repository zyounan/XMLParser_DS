#pragma once
#include <curses.h>
namespace cxxcurses {
[[nodiscard]] inline std::pair<int, int> get_current_yx(
    WINDOW* window = stdscr) noexcept {
    auto y{0};
    auto x{0};
    getyx(window, y, x);

    return {y, x};
}
[[nodiscard]] inline std::pair<int, int> get_max_yx(WINDOW* window = stdscr) noexcept {
    auto y{0};
    auto x{0};
    getmaxyx(window, y, x);

    return {y, x};
}
[[nodiscard]] inline auto get_attrs(::WINDOW* window, const int y,
                             const int x) noexcept {
    const auto col{mvwinch(window, y, x) & A_COLOR};
    const auto attr{mvwinch(window, y, x) & A_ATTRIBUTES};

    return col | attr;
}
inline int flush(){
    return ::refresh();
}
}  // namespace cxxcurses