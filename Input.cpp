#include "Input.h"

Input::Input() {
    dInput = nullptr;
    keyboard = nullptr;
    mouse = nullptr;
    ZeroMemory(keys, sizeof(keys));
    ZeroMemory(prevKeys, sizeof(prevKeys));
    ZeroMemory(&mouseState, sizeof(mouseState));
    ZeroMemory(&prevMouseState, sizeof(prevMouseState));
}

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

    memcpy(prevKeys, keys, sizeof(keys));
    prevMouseState = mouseState;


    HRESULT hr = keyboard->GetDeviceState(sizeof(keys), (LPVOID)&keys);
    if (FAILED(hr)) {
        if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
            keyboard->Acquire(); // reacquire if tabbed
        }
    }

    hr = mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouseState);
    if (FAILED(hr)) {
        if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
            mouse->Acquire();
        }
    }
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