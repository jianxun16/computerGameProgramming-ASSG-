#pragma once
#include <cstdio>
#include <cstdarg>

// Tiny action logger for the console (CLI) window. main() keeps the console
// visible; every player action prints one line here so you can watch what the
// player is doing (move, jump, attack, pause, change volume, ...).
//
// Messages are in English on purpose: the default Windows console code page
// mangles Chinese unless UTF-8 is set up, so plain ASCII stays readable
// everywhere.
inline void GameLog(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("> ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
    fflush(stdout);   // show immediately, don't wait for the buffer to fill
}
