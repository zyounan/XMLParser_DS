#include "ui.h"
#include <ncurses.h>
#include "cxxcurses/cxxcurses.hpp"
#include "stdc++.h"
namespace UI {
const char *versionInfo = "v0.1", *lastBuild = "2020/03";
const char* aboutInfo =
    "Xml file editor - A project for Data Structure course\n";
template <typename... T>
void __alignCenter(int bx, int by, const std::string& str, T&&... args) {
    using cxxcurses::cursor;
    using std::max;
    if (bx == -1 || by == -1)
        by = cursor::getCursor(cursor::getType::Y),
        bx = cursor::getCursor(cursor::getType::maxX) >> 1;
    int len = (int)str.length() >> 1;
    bx = max(0, bx - len);
    // std::cout << bx << " " << by << std::endl;
    cxxcurses::print(by, bx)(str, std::forward<T>(args)...);
    cxxcurses::flush();
}
void moveCenter() {
    int x = getmaxx(stdscr), y = getmaxy(stdscr);
    // printf("*%d %d\n", x, y);
    wmove(stdscr, (y >> 1), (x >> 1));
}
void alignCenter(const std::string& str) {
    using std::min;
    using std::string;
    size_t pos = 0;
    moveCenter();
    while (pos != string::npos) {
        size_t nxt = str.find('\n', pos + 1);
        if (nxt != string::npos) {
            __alignCenter(-1, -1, str.substr(pos, nxt - pos) + "\n");
        }
        pos = nxt;
    }
}
void showAbout() {
    using namespace cxxcurses;
    using std::string;
    cxxcurses::cursor::getCursor(cursor::getType::maxY);
    cursor::set_visibility(cursor::visibility::invisible);
    alignCenter(aboutInfo);
    __alignCenter(-1, -1, "Version : {yB}  Build : {yB}\n", versionInfo,
                  lastBuild);
    __alignCenter(-1, -1, "{cU}", "By : zyounan\n");
    __alignCenter(-1, -1, "type {r} to show help info.\n", ":help");
    __alignCenter(-1, -1, "type {r} to show about info.\n", ":about");
}
void init() {
    using std::cin;
    using std::string;
    cxxcurses::initializer init;
    showAbout();
}
void loop() {
    using namespace cxxcurses;
    set_echo(true);
    cbreak();
    char buffer[255];
    cursor::set_visibility(cursor::visibility::normal);
    int y = cursor::getCursor(cursor::getType::maxY) - 1;
    while (1) {
        mvscanw(y, 0, "%s", buffer + 1);
        while (buffer[1] != ':') {
            clrtoeol();
            print(y, 0)("{rS}",
                        "Invalid command(s) without starting with \":\"!");
            refresh();
            getch();
            cursor::setCursor(y, 0);
            clrtoeol();
            mvscanw(y, 0, "%s", buffer + 1);
        }
        clrtoeol();
        // do something..

        refresh();
    }
    endwin();
}
};  // namespace UI