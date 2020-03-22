#include "ui.h"
#include "cxxcurses/cxxcurses.hpp"
#include "reflect.h"
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
template <typename... T>
void __alignRight(int bx, int by, const std::string& str, T&&... args) {
    using cxxcurses::cursor;
    using std::max;
    size_t nn = str.length();
    if (bx == -1) bx = cursor::getCursor(cursor::getType::maxX) - nn - 2;
    // 我们假设屏幕至少有2个像素高
    if (by == -1) by = 2;

    bx = max(0, bx);
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
    __alignCenter(-1, -1, "type {r} to switch into edit mode.\n", "Ctrl + E");
}
void init() {
    using std::cin;
    using std::string;
    using namespace Reflect;
    cxxcurses::initializer init;
    showAbout();
    initReflect();
}
static bool commandInsertMode = false;
static bool editMode = false;
const int _bufferSize = 255;
static int lastX = 0, lastY = 0;
unsigned int buffer[_bufferSize];
void printText() {}
void processEdit(int ch) {
    using namespace cxxcurses;
    int mxy = cursor::getCursor(cursor::getType::maxY) - 2,
        mxx = cursor::getCursor(cursor::getType::maxX) - 10;
    ::clear();
    flush();
    printText();
    cursor::setCursor(lastY, lastX);
    switch (ch) {
        case '\n':
        case '\r': {
            int y = cursor::getCursor(cursor::getType::Y);
            y = std::min(y,mxy);
            
            cursor::setCursor(y,-1);

        }
        case KEY_BACKSPACE:
        case '\b': {
        }
        case KEY_DC: {
        }
        case KEY_LEFT: {
        }
        case KEY_RIGHT: {
        }
        case KEY_UP: {
        }
        case KEY_DOWN: {
        }
        case KEY_HOME: {
        }
        case KEY_END: {
        }
        case KEY_EXIT: {
        }
        case CTRL('f'): {
        }
        case CTRL('b'): {
        }
        default:{

        }
    }
}
void loop() {
    using namespace cxxcurses;
    set_echo(false);
    keypad(stdscr, true);
    cbreak();
    cursor::set_visibility(cursor::visibility::high);
    int y = cursor::getCursor(cursor::getType::maxY) - 1;
    int tot = 0;
    cursor::setCursor(y, 0);
    refresh();
    while (1) {
        int c = getch();
        int _debug_NowX = cursor::getCursor(cursor::getType::X),
            _debug_NowY = cursor::getCursor(cursor::getType::Y);
        __alignRight(-1, -1, "X:{yB} Y: {yB}", _debug_NowX, _debug_NowY);
        cursor::setCursor(_debug_NowY, _debug_NowX);
        if (editMode) {
            processEdit(c);
            continue;
        }
        switch (c) {
            case '\n':
            case '\r': {
                //处理输入的命令
                commandInsertMode = false, tot = 0;
                memset(buffer, 0, sizeof buffer);
                error_check(mvinchnstr(y, 0, (buffer + 1), _bufferSize - 1),
                            "Unexpected char from screen!");
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

                    commandInsertMode = false;

                    if (cc != '\n' && cc != '\r' && cc != '\t') {
                        print(y, 0)(cc);
                        refresh();
                        // buffer[tot = 1] = cc;
                    } else
                        // buffer[tot = 0] = '\0';
                        continue;
                } else {
                    cursor::setCursor(y, 0);
                    clearToEndOfLine();
                    unsigned int* p = buffer + 1;
                    std::string res;
                    res.reserve(_bufferSize);
                    while (p != buffer + _bufferSize) {
                        res.push_back(*(p += 2));
                    }
                    res.erase(std::find_if(res.rbegin(), res.rend(),
                                           [&](char c) -> bool {
                                               return !std::isspace(c);
                                           })
                                  .base(),
                              res.end());
                    // do something..
                    refresh();
                    continue;
                }
                break;
            }
            case KEY_BACKSPACE:
            case '\b': {
                //退格键
                cursor::setCursor(-1,
                                  cursor::getCursor(cursor::getType::X) - 1);
                tot = std::max(tot - 1, 0);
                delch();
                refresh();
                break;
            }
            case KEY_DC: {
                // 删除键
                // cursor::setCursor(-1,
                //                   cursor::getCursor(cursor::getType::X) + 1);
                int x = cursor::getCursor(cursor::getType::X);
                tot = std::max(tot - 1, 0);
                delch();
                refresh();
                if (x + 1 == tot) commandInsertMode = false;
                break;
            }
            case KEY_LEFT: {
                //向左移动光标
                commandInsertMode = 1;
                cursor::set_visibility(cursor::visibility::normal);
                cursor::setCursor(-1,
                                  cursor::getCursor(cursor::getType::X) - 1);
                break;
            }
            case KEY_RIGHT: {
                //向右移动光标
                commandInsertMode = 1;
                cursor::set_visibility(cursor::visibility::normal);
                int x = cursor::getCursor(cursor::getType::X) + 1;
                if (x <= tot) {
                    cursor::setCursor(-1, x);
                }
                if (x > tot) commandInsertMode = false;
                break;
            }
            case KEY_HOME: {
                //返回到当前行首
                commandInsertMode = tot;
                cursor::setCursor(y, 0);
                break;
            }
            case KEY_END: {
                //返回到当前行尾部
                commandInsertMode = false;
                cursor::setCursor(y, tot);
                break;
            }
            case CTRL('e'): {
                //切换到编辑模式
                editMode = true;
                cursor::setCursor(y, 0);
                clearToEndOfLine();
                cursor::set_visibility(cursor::visibility::normal);
                commandInsertMode = false, tot = 0;
                memset(buffer, 0, sizeof buffer);
                break;
            }
            case KEY_EXIT: {
                //切换到命令模式
                editMode = false;
                cursor::setCursor(y, 0);
                clearToEndOfLine();
                cursor::set_visibility(cursor::visibility::normal);
                commandInsertMode = false, tot = 0;

                break;
            }
            default: {
                //自由输入字符
                tot = std::min(tot + 1, _bufferSize);
                if (commandInsertMode) {
                    ::insnstr((const char*)(&c), 1);
                    cursor::setCursor(
                        -1, cursor::getCursor(cursor::getType::X) + 1);
                } else {
                    print((char)c);
                    // buffer[tot] = c;
                }
                refresh();
                break;
            }
        }  // end switch
    }      // end main loop
    endwin();
}
};  // namespace UI