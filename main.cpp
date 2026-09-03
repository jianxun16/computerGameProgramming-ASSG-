#define WIN32_LEAN_AND_MEAN
#define PI 3.14159
#include <Windows.h>
#include <iostream>

#include "GameEngine.h"
#include "MenuState.h"

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

    // GameMenu sits at the bottom of the state stack; it pushes PlayState on
    // Play, and Victory / Game Over pop the whole game back down to it.
    engine.GetStateManager()->PushState(new MenuState());

    engine.Run();
    engine.Shutdown();

    return 0;
}