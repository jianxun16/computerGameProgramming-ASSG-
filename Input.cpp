#include "Input.h"

bool Input::InitializeInput(HINSTANCE hInstance, HWND hWnd) {
    DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&dInput, NULL);

    dInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
    keyboard->SetDataFormat(&c_dfDIKeyboard);
    keyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    dInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
    mouse->SetDataFormat(&c_dfDIMouse);
    mouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    return true;
}

void Input::PollDeviceStates() {
    keyboard->Acquire();
    keyboard->GetDeviceState(256, keys);

    mouse->Acquire();
    mouse->GetDeviceState(sizeof(mouseState), &mouseState);
}

void Input::CleanUpInput() {
    if (keyboard) {
        keyboard->Unacquire();
        keyboard->Release();
        keyboard = nullptr;
    }
    if (mouse) {
        mouse->Unacquire();
        mouse->Release();
        mouse = nullptr;
    }
    if (dInput) {
        dInput->Release();
        dInput = nullptr;
    }
}