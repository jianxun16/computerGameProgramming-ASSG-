#include "Game.h"
#include <Windows.h>
#include <cstdio>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

int main()
{
    // Keep the console visible: it doubles as a live "action log" (CLI) that
    // shows what the player is doing (see GameLog). FMOD / DirectX also print
    // the occasional library message here.
    ShowWindow(GetConsoleWindow(), SW_SHOW);
    SetConsoleTitleA("Player Action Log");
    printf("=============================\n");
    printf("   Player Action Log (CLI)\n");
    printf("=============================\n");
    fflush(stdout);

    Game game;
    game.init();
    game.run();
    return 0;   // Game's destructor cleans everything up
}
