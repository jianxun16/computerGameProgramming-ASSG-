#define WIN32_LEAN_AND_MEAN
#define PI 3.14159
#include <Windows.h>
#include <iostream>

#include "GameEngine.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using namespace std;

int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    cout << "Game Running..." << endl;

    GameEngine engine;
    if (!engine.Initialize(hInstance, 1280, 720, false, 60)) {
        MessageBox(NULL, "Engine failed to initialize.", "Error", MB_OK);
        return 1;
    }

    // ------------------------------------------------------------------
    // TODO: push the first state here once MenuState / PlayState exist.
    // Example (once you have MenuState):
    //     #include "MenuState.h"
    //     engine.GetStateManager()->PushState(new MenuState());
    //
    // With nothing pushed the window opens black and closes on X — useful
    // for verifying the engine boots cleanly before adding game code.
    // ------------------------------------------------------------------

    engine.Run();
    engine.Shutdown();

    return 0;
}