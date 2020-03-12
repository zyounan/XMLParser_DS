// #ifndef CXXCURSES_ATTRIBUTE_HPP
// #define CXXCURSES_ATTRIBUTE_HPP
#pragma once
#include <unordered_map>
namespace cxxcurses {
enum class attribute {
    normal = A_NORMAL,
    standout = A_STANDOUT,
    underline = A_UNDERLINE,
    reverse = A_REVERSE,
    blink = A_BLINK,
    dim = A_DIM,
    bold = A_BOLD,
    protect = A_PROTECT,
    invisible = A_INVIS
};
const std::unordered_map<char, attribute> char_to_attribute{
    {'N', attribute::normal},    {'S', attribute::standout},
    {'U', attribute::underline}, {'R', attribute::reverse},
    {'X', attribute::blink},     {'D', attribute::dim},
    {'B', attribute::bold},      {'P', attribute::protect},
    {'I', attribute::invisible}};
template <typename T, typename... Attrs>
void apply_attributes(WINDOW* window, T attr, Attrs... attrs) noexcept {
    using fuck_cxx11_no_fold_expression =
        typename std::common_type<Attrs...>::type;
    std::initializer_list<fuck_cxx11_no_fold_expression> tmp{(0, attrs)...};
    fuck_cxx11_no_fold_expression ans = {};
    for (auto& x : tmp) ans |= x;
    ::wattron(window, ans);
}
inline void apply_attributes(WINDOW* window,
                      const std::vector<attribute>& attrs) noexcept {
    for (const auto& attr : attrs) {
        ::wattron(window, static_cast<chtype>(attr));
    }
}
}  // namespace cxxcurses
// #endif  // CXXCURSES_ATTRIBUTE_HPP
