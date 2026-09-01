#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#include <dinput.h>

// Owns the DirectInput keyboard and mouse. Poll once per frame with update();
// the rest of the game queries it (isKeyDown / mouse).
class InputManager
{
public:
    InputManager();
    ~InputManager();

    bool init(HWND hWnd);
    void update();                       // poll keyboard + mouse (current state)
    void postUpdate();                   // snapshot "previous" after input is consumed
    void release();

    bool isKeyDown(int dik) const;       // DIK_A, DIK_D, DIK_SPACE, ...
    bool isKeyPressed(int dik) const;    // pressed this frame (edge)
    bool mouseLeftDown() const;          // held this frame
    bool mouseLeftClicked() const;       // pressed this frame (edge)

    int  mouseX() const { return mouseXPos; }   // cursor pos in the window
    int  mouseY() const { return mouseYPos; }

private:
    LPDIRECTINPUT8        dInput;
    LPDIRECTINPUTDEVICE8  keyboard;
    HWND                  hwnd;          // screen -> client conversion
    BYTE                  keys[256];
    BYTE                  keysPrev[256]; // last frame, for edge detection
    bool                  mouseLeftCur;
    bool                  mouseLeftPrev;
    int                   mouseXPos;
    int                   mouseYPos;
};
