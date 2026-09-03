#pragma once
#include <cstdio>
#include <cstdarg>

// Tiny action logger for the console: one line per player action. English/ASCII
// on purpose, since the default Windows console mangles Chinese without UTF-8.
inline void GameLog(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("> ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
    fflush(stdout);   // show immediately
}
