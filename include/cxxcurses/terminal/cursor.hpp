#pragma once
namespace cxxcurses {
struct cursor {
    enum class visibility { invisible = 0, normal = 1, high = 2 };
    enum class getType { X = 0, Y = 1, maxX = 2, maxY = 3 };
    static int getCursor(getType t) {
        switch (t) {
            case getType::X:
                return getcurx(stdscr);
            case getType::Y:
                return getcury(stdscr);
            case getType::maxX:
                return getmaxx(stdscr);
            case getType::maxY:
                return getmaxy(stdscr);
        }
        return -1;
    }
    static void setCursor(int y = -1,int x = -1){
        auto pa = get_current_yx(),pb = get_max_yx();
        if(y < 0 || y > pb.first)   y = std::min(pa.first,pb.first);
        if(x < 0 || x > pb.second)   x = std::min(pa.second,pb.second);
        move(y,x);
    }
    static void set_visibility(visibility v) {
        error_check(::curs_set(static_cast<int>(v)),
                    "Your terminal does not support specified visibility mode");
    }
};
}  // namespace cxxcurses
