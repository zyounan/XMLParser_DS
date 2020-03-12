#pragma once
// #include "cxxcurses/cxxcurses.hpp"
#include <ncurses.h>
#include "stdc++.h"
#include "reflect.h"
namespace UI {
void __alignCenter(const std::string&, int, int);
void moveCenter();
void alignCenter(const std::string&);
void showAbout();
void init();
void loop();
}  // namespace UI