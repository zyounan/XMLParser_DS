#pragma once
#include <ncurses.h>
#include "stdc++.h"
namespace UI {
void __alignCenter(const std::string&, int, int);
void moveCenter();
void alignCenter(const std::string&);
void showAbout();
void init();
};  // namespace UI