#include "GameWindow.h"

LRESULT CALLBACK GameWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

bool GameWindow::InitializeWindow(HINSTANCE hInst, int width, int height, bool fullscreen) {
    hInstance = hInst;

    // win32
    WNDCLASS wndClass;
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hInstance = hInstance;
    wndClass.lpfnWndProc = WindowProc;
    wndClass.lpszClassName = "OOPWindow";

    RegisterClass(&wndClass);

    DWORD style = fullscreen ? WS_POPUP : WS_OVERLAPPEDWINDOW;
    hWnd = CreateWindowEx(0, "OOPWindow", "DirectX 9 OOP Game", style,
        0, 100, width, height, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, 1);

    // directX
    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    ZeroMemory(&d3dPP, sizeof(d3dPP));

    d3dPP.Windowed = !fullscreen;
    d3dPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dPP.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dPP.BackBufferCount = 1;
    d3dPP.BackBufferWidth = width;
    d3dPP.BackBufferHeight = height;
    d3dPP.hDeviceWindow = hWnd;

    d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dPP, &d3dDevice);

    return true;
}

bool GameWindow::ProcessMessages() {
    MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}

void GameWindow::CleanUpWindow() {
    if (d3dDevice) d3dDevice->Release();
    if (d3d) d3d->Release();
    UnregisterClass("OOPWindow", hInstance);
}