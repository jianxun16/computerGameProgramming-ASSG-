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
    // Read the CURRENT keyboard state. The "previous" snapshot used for edge
    // detection is taken in postUpdate() (after a state actually consumes input),
    // NOT here -- otherwise, on frames where the fixed-step loop runs no update
    // (display FPS > 32), a key/mouse edge would be snapshotted away unseen.
    keyboard->Acquire();
    keyboard->GetDeviceState(sizeof(keys), keys);

    // Mouse left button (current) + cursor position in client-area pixels.
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
    // Called once per consumed simulation step: the current state becomes the
    // "previous" for the next edge comparison.
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
    // Down this frame, up last frame = a fresh press (one event per keystroke).
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
