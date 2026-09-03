#include "Cheat.h"
#include "GameLog.h"
#include <conio.h>   // _kbhit / _getche (non-blocking console input)
#include <string>
#include <cctype>

namespace
{
    bool        g_enabled = false;    // god mode on/off
    std::string g_buffer;             // characters typed so far this line
    const char* CHEAT_CODE = "idkfa"; // type this + Enter in the console
}

namespace Cheat
{
    bool enabled() { return g_enabled; }

    void pollConsole()
    {
        // Drain everything typed since last frame (non-blocking, so the game
        // keeps running whether or not the console has focus).
        while (_kbhit())
        {
            int ch = _getche();   // echo, so the typed code is visible

            if (ch == '\r' || ch == '\n')          // Enter -> check the line
            {
                std::string typed = g_buffer;
                for (size_t i = 0; i < typed.size(); i++)
                    typed[i] = (char)tolower((unsigned char)typed[i]);

                if (typed == CHEAT_CODE)
                {
                    g_enabled = !g_enabled;
                    GameLog(g_enabled ? "CHEAT MODE ON  (god mode: you can't die)"
                                      : "CHEAT MODE OFF");
                }
                g_buffer.clear();
            }
            else if (ch == '\b')                    // backspace
            {
                if (!g_buffer.empty()) g_buffer.pop_back();
            }
            else if (ch >= 32 && ch < 127)          // printable char
            {
                g_buffer.push_back((char)ch);
                if (g_buffer.size() > 32)           // keep the buffer small
                    g_buffer.erase(0, g_buffer.size() - 32);
            }
        }
    }
}
