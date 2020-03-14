#include "ui.h"
#include "cxxcurses/cxxcurses.hpp"
#include "stdc++.h"
#include "reflect.h"

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
    using namespace Reflect;
    cxxcurses::initializer init;
    showAbout();
    initReflect();
}
void loop() {
    //TODO : 维护字符串缓冲区
    static bool insertMode = false;
    using namespace cxxcurses;
    set_echo(false);
    keypad(stdscr, true);
    // cbreak();
    char buffer[255];
    cursor::set_visibility(cursor::visibility::high);
    int y = cursor::getCursor(cursor::getType::maxY) - 1;
    int tot = 0;
    cursor::setCursor(y, 0);
    refresh();
    while (1) {
        int c = getch();
        switch (c) {
            case '\n':
            case '\r': {
                if (buffer[1] != ':') {
                    cursor::setCursor(y, 0);
                    clearToEndOfLine();
                    print(y, 0)("{rS}",
                                "Invalid command(s) starting without \":\"!");
                    cursor::set_visibility(cursor::visibility::invisible);
                    cursor::setCursor(y, 0);
                    refresh();
                    char cc = getch();
                    clearToEndOfLine();
                    cursor::set_visibility(cursor::visibility::high);

                    insertMode = false;

                    if (cc != '\n' && cc != '\r' && cc != '\t') {
                        print(y, 0)(cc);
                        refresh();
                        buffer[tot = 1] = cc;
                    } else
                        buffer[tot = 0] = '\0';
                    continue;
                } else {
                    cursor::setCursor(y, 0);
                    clearToEndOfLine();
                    // do something..
                    buffer[tot = 0] = '\0';
                    refresh();
                    continue;
                }
                break;
            }
            case KEY_BACKSPACE:
            case '\b': {
                cursor::setCursor(-1,
                                  cursor::getCursor(cursor::getType::X) - 1);
                delch();
                refresh();
                break;
            }
            case KEY_DC: {
                // DELETE
                // cursor::setCursor(-1,
                //                   cursor::getCursor(cursor::getType::X) + 1);
                delch();
                refresh();
                break;
            }
            case KEY_LEFT: {
                insertMode = 1;
                cursor::set_visibility(cursor::visibility::normal);
                cursor::setCursor(-1,
                                  cursor::getCursor(cursor::getType::X) - 1);
                break;
            }
            case KEY_RIGHT: {
                insertMode = 1;
                cursor::set_visibility(cursor::visibility::normal);
                cursor::setCursor(-1,
                                  cursor::getCursor(cursor::getType::X) + 1);
                break;
            }
            default: {
                if (++tot >= 255) {
                    cursor::setCursor(y, 0);
                    clearToEndOfLine();
                    print(y, 0)("{yB}", "TOO LONG COMMANDS!!");
                    buffer[tot = 0] = '\0';
                    continue;
                }
                if (insertMode) {
                    ::insnstr((const char*)&c, 1);
                } else {
                    print((char)c);
                    buffer[tot] = c;
                }
                refresh();
                break;
            }
        }  // end switch
    }      // end main loop
    endwin();
}
};  // namespace UI