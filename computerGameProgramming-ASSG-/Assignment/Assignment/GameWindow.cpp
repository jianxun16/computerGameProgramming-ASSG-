#include "GameWindow.h"

GameWindow::GameWindow()
{
    hWnd = NULL;
    ZeroMemory(&wndClass, sizeof(wndClass));
    ZeroMemory(&msg, sizeof(msg));
    w = 0;
    h = 0;
}

GameWindow::~GameWindow()
{
    release();
}

LRESULT CALLBACK GameWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    // NOTE: Esc is the pause key now (handled by the game states via
    // DirectInput), so it must NOT quit here. Close with the window's X.

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

bool GameWindow::create(const char* title, int width, int height)
{
    w = width;
    h = height;

    ZeroMemory(&wndClass, sizeof(wndClass));
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hInstance = GetModuleHandle(NULL);
    wndClass.lpfnWndProc = WndProc;
    wndClass.lpszClassName = "MyWindow";
    wndClass.style = CS_HREDRAW | CS_VREDRAW;

    RegisterClass(&wndClass);

    hWnd = CreateWindowEx(0, wndClass.lpszClassName, title, WS_OVERLAPPEDWINDOW,
                          0, 100, width, height, NULL, NULL, GetModuleHandle(NULL), NULL);
    ShowWindow(hWnd, 1);

    ZeroMemory(&msg, sizeof(msg));
    return hWnd != NULL;
}

bool GameWindow::processMessages()
{
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return false;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}

void GameWindow::release()
{
    if (hWnd != NULL)
    {
        UnregisterClass(wndClass.lpszClassName, GetModuleHandle(NULL));
        hWnd = NULL;
    }
}
