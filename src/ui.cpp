#include "ui.h"
#include <ncurses.h>
#include "stdc++.h"
namespace UI {
const char *versionInfo = "v0.1", *lastBuild = "2020/03";
const char* aboutInfo =
    "Xml file editor - A project for Data Structure course\n";
void __alignCenter(const std::string& str, int bx = -1, int by = -1) {
    using std::max;
    if (bx == -1 || by == -1) by = getcury(stdscr), bx = getmaxx(stdscr) >> 1;
    int len = (int)str.length() >> 1;
    bx = max(0, bx - len);
    // std::cout << bx << " " << by << std::endl;
    wmove(stdscr, by, bx);
    printw(str.c_str());
    wmove(stdscr, by + 1, bx);
    refresh();
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
            __alignCenter(str.substr(pos, nxt - pos));
        }
        pos = nxt;
    }
}
void showAbout() {
    using std::string;
    initscr();
    curs_set(0);
    attron(A_BOLD);
    alignCenter(aboutInfo);
    attroff(A_BOLD);
    std::stringstream ss;
    ss << "Version : " << versionInfo << "  Lastbuild : " << lastBuild << "\n";
    __alignCenter(ss.str());
    attron(A_STANDOUT);
    ss.str(string());
    ss << "By : zyounan";
    __alignCenter(ss.str());
    attroff(A_STANDOUT);
    ss.str(string());
    ss << "\n";
    __alignCenter(ss.str());
    ss.str(string());
    ss << "type :about to show about info";
    __alignCenter(ss.str());
    ss.str(string());
    ss << "type :help to show help info";
    __alignCenter(ss.str());
}
void init() {
    using std::cin;
    using std::string;
    curs_set(1);
    showAbout();
    cin.get();
    endwin();
}
};  // namespace UI