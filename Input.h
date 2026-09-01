#pragma once
#include <dinput.h>

class Input {
private:
    LPDIRECTINPUT8 dInput;
    LPDIRECTINPUTDEVICE8 keyboard;
    LPDIRECTINPUTDEVICE8 mouse;
    BYTE keys[256];
    DIMOUSESTATE mouseState;

public:
    bool InitializeInput(HINSTANCE hInstance, HWND hWnd);
    void PollDeviceStates();
    void CleanUpInput();

    bool IsKeyPressed(int dikCode) { return (keys[dikCode] & 0x80) != 0; }
};