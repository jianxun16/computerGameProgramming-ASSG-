// author by limzhenxing

#pragma once

// Console cheat code. Click the "Player Action Log" console window, type the
// code (default "idkfa") and press Enter to toggle GOD MODE on/off. While it's
// on the player can't be killed (boss balls, spikes and the pit are ignored).
//
// pollConsole() must be called once per frame from the main loop; enabled() is
// queried by the gameplay states.
namespace Cheat
{
    bool enabled();       // is god mode currently on?
    void pollConsole();   // read console keystrokes, toggle on the code + Enter
}
