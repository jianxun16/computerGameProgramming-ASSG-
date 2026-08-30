#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#include <dinput.h>

// Owns the DirectInput keyboard and the mouse. Poll once per frame with
// update(); the rest of the game asks it questions (isKeyDown / mouse).
class InputManager
{
public:
    InputManager();
    ~InputManager();

    bool init(HWND hWnd);
    void update();                       // poll keyboard + mouse each frame (current state only)
    void postUpdate();                   // snapshot "previous" AFTER a state consumes input,
                                         // so key/mouse edges survive frames with no fixed-step tick
    void release();

    bool isKeyDown(int dik) const;       // DIK_A, DIK_D, DIK_SPACE, ...
    bool isKeyPressed(int dik) const;    // pressed this frame (edge, for toggles like pause)
    bool mouseLeftDown() const;          // held this frame
    bool mouseLeftClicked() const;       // pressed this frame (edge)

    int  mouseX() const { return mouseXPos; }   // cursor position inside the window
    int  mouseY() const { return mouseYPos; }

private:
    LPDIRECTINPUT8        dInput;
    LPDIRECTINPUTDEVICE8  keyboard;
    HWND                  hwnd;          // to convert screen -> client cursor coords
    BYTE                  keys[256];
    BYTE                  keysPrev[256]; // last frame, for key-edge detection
    bool                  mouseLeftCur;
    bool                  mouseLeftPrev;
    int                   mouseXPos;
    int                   mouseYPos;
};
