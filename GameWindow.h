#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Owns the Win32 window and its message pump.
class GameWindow
{
public:
    GameWindow();
    ~GameWindow();

    bool create(const char* title, int width, int height);
    bool processMessages();     // pump messages; returns false on WM_QUIT
    void release();

    HWND handle() const { return hWnd; }
    int  width()  const { return w; }
    int  height() const { return h; }

private:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    HWND     hWnd;
    WNDCLASS wndClass;
    MSG      msg;
    int      w;
    int      h;
};
