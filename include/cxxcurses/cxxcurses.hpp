#pragma once
#include <curses.h>
#include "print.hpp"
#include "terminal.hpp"

#define CTRL(c) ((c)&0x1F)
#define ALT(c) ((c) | 0xa0)