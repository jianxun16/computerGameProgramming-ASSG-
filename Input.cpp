#include "Input.h"

Input::Input() {
    dInput = NULL;
    keyboard = NULL;
    mouse = NULL;
    ZeroMemory(keys, sizeof(keys));
    ZeroMemory(prevKeys, sizeof(prevKeys));
    ZeroMemory(&mouseState, sizeof(mouseState));
    ZeroMemory(&prevMouseState, sizeof(prevMouseState));

    hWnd = NULL;
    mouseX = 0;
    mouseY = 0;
}

bool Input::InitializeInput(HINSTANCE hInstance, HWND windowHandle) {
    hWnd = windowHandle;
    DirectInput8Create(GetModuleHandle(NULL), 0x0800, IID_IDirectInput8, (void**)&dInput, NULL);

    dInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
    keyboard->SetDataFormat(&c_dfDIKeyboard);
    keyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    keyboard->Acquire();  

    dInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
    mouse->SetDataFormat(&c_dfDIMouse);
    mouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    mouse->Acquire();   

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

    if (hWnd != NULL) {
        POINT pt;
        GetCursorPos(&pt);           
        ScreenToClient(hWnd, &pt);
        mouseX = pt.x;
        mouseY = pt.y;
    }
}

void Input::CleanUpInput() {
    if (keyboard) {
        keyboard->Unacquire();
        keyboard->Release();
        keyboard = NULL;
    }
    if (mouse) {
        mouse->Unacquire();
        mouse->Release();
        mouse = NULL;
    }
    if (dInput) {
        dInput->Release();
        dInput = NULL;
    }
}