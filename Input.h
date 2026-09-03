#pragma once
#include <dinput.h>

class Input {
private:
    LPDIRECTINPUT8 dInput;
    LPDIRECTINPUTDEVICE8 keyboard;
    LPDIRECTINPUTDEVICE8 mouse;

    BYTE keys[256];
    BYTE prevKeys[256];

    DIMOUSESTATE mouseState;
    DIMOUSESTATE prevMouseState;

    HWND hWnd;
    int mouseX, mouseY;

public:
    Input();
    bool InitializeInput(HINSTANCE hInstance, HWND hWnd);
    void PollDeviceStates();
    void CleanUpInput();

    // Keyboard Checks
    bool IsKeyDown(int dikCode) { return (keys[dikCode] & 0x80) != 0; }
    bool IsKeyJustPressed(int dikCode) { return (keys[dikCode] & 0x80) != 0 && (prevKeys[dikCode] & 0x80) == 0; }

    // Mouse Checks
    int GetMouseX() { return mouseX; }
    int GetMouseY() { return mouseY; }
    long GetMouseDX() const { return mouseState.lX; }
    long GetMouseDY() const { return mouseState.lY; }
    bool IsMouseButtonDown(int button) { return (mouseState.rgbButtons[button] & 0x80) != 0; }
    bool IsMouseButtonJustPressed(int button) { return (mouseState.rgbButtons[button] & 0x80) != 0 && (prevMouseState.rgbButtons[button] & 0x80) == 0; }
};