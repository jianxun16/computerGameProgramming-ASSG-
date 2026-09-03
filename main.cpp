#define WIN32_LEAN_AND_MEAN
#define PI 3.14159
#include <Windows.h>
#include <iostream>

#include "GameEngine.h"
#include "PlayState.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "fmod_vc.lib")

using namespace std;

int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    hInstance = GetModuleHandle(NULL);
    cout << "Game Running..." << endl;

    GameEngine engine;
    if (!engine.Initialize(hInstance, 1280, 720, false, 60)) {
        MessageBox(NULL, "Engine failed to initialize.", "Error", MB_OK);
        cout << "Engine Failed to initialize";
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
    engine.GetStateManager()->PushState(new PlayState("Assets/Map/Map1.txt", "bgm_stage1"));

    engine.Run();
    engine.Shutdown();

    return 0;
}