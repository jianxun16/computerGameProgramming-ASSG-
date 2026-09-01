#include "InputManager.h"

InputManager::InputManager()
{
    dInput = NULL;
    keyboard = NULL;
    hwnd = NULL;
    ZeroMemory(keys, sizeof(keys));
    ZeroMemory(keysPrev, sizeof(keysPrev));
    mouseLeftCur = false;
    mouseLeftPrev = false;
    mouseXPos = 0;
    mouseYPos = 0;
}

InputManager::~InputManager()
{
    release();
}

bool InputManager::init(HWND hWnd)
{
    hwnd = hWnd;

    // Create the DirectInput object.
    HRESULT hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
                                    IID_IDirectInput8, (void**)&dInput, NULL);
    if (FAILED(hr))
        return false;

    // Create + configure the keyboard device.
    dInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
    keyboard->SetDataFormat(&c_dfDIKeyboard);
    keyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    keyboard->Acquire();
    return true;
}

void InputManager::update()
{
    // Read the current keyboard state; the "previous" snapshot is taken in
    // postUpdate() so edges survive frames with no fixed-step tick.
    keyboard->Acquire();
    keyboard->GetDeviceState(sizeof(keys), keys);

    // Mouse left button + cursor position (client pixels).
    mouseLeftCur = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

    POINT p;
    if (GetCursorPos(&p))
    {
        if (hwnd != NULL) ScreenToClient(hwnd, &p);
        mouseXPos = p.x;
        mouseYPos = p.y;
    }
}

void InputManager::postUpdate()
{
    // Once per consumed step: current becomes "previous" for edge detection.
    memcpy(keysPrev, keys, sizeof(keys));
    mouseLeftPrev = mouseLeftCur;
}

void InputManager::release()
{
    if (keyboard != NULL)
    {
        keyboard->Unacquire();
        keyboard->Release();
        keyboard = NULL;
    }
    if (dInput != NULL)
    {
        dInput->Release();
        dInput = NULL;
    }
}

bool InputManager::isKeyDown(int dik) const
{
    return (keys[dik] & 0x80) != 0;
}

bool InputManager::isKeyPressed(int dik) const
{
    // Down now, up last frame = a fresh press.
    return (keys[dik] & 0x80) != 0 && (keysPrev[dik] & 0x80) == 0;
}

bool InputManager::mouseLeftDown() const
{
    return mouseLeftCur;
}

bool InputManager::mouseLeftClicked() const
{
    return mouseLeftCur && !mouseLeftPrev;
}
