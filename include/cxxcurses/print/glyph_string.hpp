#pragma once
#include <sstream>
#include <vector>
#include "color_pair.hpp"
#include "glyph.hpp"

namespace cxxcurses {
//  用于解析格式化字符串的标记类
class glyph_string : public std::vector<glyph> {
   public:
    explicit glyph_string(const std::string& string,
                          color_pair color = color::white) {
        for (const auto& c : string) {
            emplace_back(c, color);
        }
    }
    void print(const int y, const int x) const noexcept {
        int i = 0;
        for (const auto& g : *this) {
            g.print(y, x + i++);
        }
    }
};
// TODO: refactor and optimize
template <typename T>
glyph_string parse(glyph_string parsed, const T& arg) {
    auto ss = std::stringstream();
    auto glyphColor = color_pair(color::white);
    auto glyphAttr = std::vector<attribute>();
    const auto openPos =
        std::find(std::begin(parsed), std::end(parsed), glyph('{'));
    const auto closePos =
        std::find(std::begin(parsed), std::end(parsed), glyph('}'));
    const auto format_len = closePos - openPos;  // std::distance

    if (openPos != std::end(parsed) && closePos != std::end(parsed) &&
        openPos < closePos) {
        auto attr = std::vector<glyph>(openPos, closePos);
        if (!attr.empty()) {
            for (const auto& flag : attr) {
                if (char_to_color.count(flag.data())) {
                    glyphColor = color_pair{char_to_color.at(flag.data())};
                }
                if (char_to_attribute.count(flag.data())) {
                    glyphAttr.push_back(char_to_attribute.at(flag.data()));
                }
            }
        }
        ss << arg;
        auto parsed_arg{glyph_string{ss.str(), glyphColor}};
        for (auto& g : parsed_arg) {
            g.set_attributes(glyphAttr);
        }
        auto it = parsed.insert(closePos + 1, std::begin(parsed_arg),
                                std::end(parsed_arg));
        parsed.erase(it - format_len - 1, it);
    }
    return parsed;
}
}  // namespace cxxcurses
