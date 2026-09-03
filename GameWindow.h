#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>

class GameWindow {
private:
    HWND hWnd;
    HINSTANCE hInstance;

    IDirect3D9* d3d;
    IDirect3DDevice9* d3dDevice;
    D3DPRESENT_PARAMETERS d3dPP;

    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
    bool InitializeWindow(HINSTANCE hInst, int width, int height, bool fullscreen);
    bool ProcessMessages();
    void CleanUpWindow();

    HWND GetWindowHandle() const { return hWnd; }
    IDirect3DDevice9* GetD3DDevice() const { return d3dDevice; }
};