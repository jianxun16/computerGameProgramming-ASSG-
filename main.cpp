#include "Game.h"
#include <Windows.h>
#include <cstdio>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

int main()
{
    // Keep the console visible: it's the live action log (see GameLog).
    ShowWindow(GetConsoleWindow(), SW_SHOW);
    SetConsoleTitleA("Player Action Log");
    printf("=============================\n");
    printf("   Player Action Log (CLI)\n");
    printf("=============================\n");
    fflush(stdout);

    Game game;
    game.init();
    game.run();
    return 0;   // Game's destructor cleans up
}
