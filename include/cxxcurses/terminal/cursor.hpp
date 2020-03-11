#pragma once
namespace cxxcurses {
struct cursor {
    enum class visibility { invisible = 0, normal = 1, high = 2 };

    static void set_visibility(visibility v) {
        error_check(::curs_set(static_cast<int>(v)),
                    "Your terminal does not support specified visibility mode");
    }
};
}  // namespace cxxcurses
