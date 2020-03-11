#pragma once
#include <curses.h>
#include <exception>
namespace cxxcurses {
class curses_error : public std::runtime_error {
   public:
    explicit curses_error(const std::string& arg) : runtime_error{arg} {}
};
void error_check(int return_value, const std::string& what_msg) {
    if (return_value == ERR) {
        throw curses_error{what_msg};
    }
}
int set_echo(bool mode) noexcept {
    if (mode) {
        return ::echo();
    }

    return ::noecho();
}
}  // namespace cxxcurses
#include "terminal/cursor.hpp"
#include "terminal/initializer.hpp"
