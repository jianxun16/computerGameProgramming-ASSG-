#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

// Owns the Direct3D 9 device and the shared sprite brush. Wraps a frame in
// beginFrame() / endFrame(); states draw with sprite() in between.
class Graphics
{
public:
    Graphics();
    ~Graphics();

    bool init(HWND hWnd, int width, int height);
    void beginFrame();   // Clear + BeginScene + sprite Begin
    void endFrame();     // sprite End + EndScene + Present
    void release();

    IDirect3DDevice9* device() const { return d3dDevice; }
    LPD3DXSPRITE      sprite() const { return spriteBrush; }
    int               width()  const { return screenWidth; }
    int               height() const { return screenHeight; }

private:
    IDirect3D9*           d3d9;
    IDirect3DDevice9*     d3dDevice;
    LPD3DXSPRITE          spriteBrush;
    D3DPRESENT_PARAMETERS d3dPP;
    int                   screenWidth;
    int                   screenHeight;
};
