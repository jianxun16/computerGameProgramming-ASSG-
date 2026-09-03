#include "Graphics.h"

Graphics::Graphics()
{
    d3d9 = NULL;
    d3dDevice = NULL;
    spriteBrush = NULL;
    ZeroMemory(&d3dPP, sizeof(d3dPP));
    screenWidth = 0;
    screenHeight = 0;
}

Graphics::~Graphics()
{
    release();
}

bool Graphics::init(HWND hWnd, int width, int height)
{
    screenWidth = width;
    screenHeight = height;

    d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
    if (d3d9 == NULL)
        return false;

    ZeroMemory(&d3dPP, sizeof(d3dPP));
    d3dPP.Windowed = true;
    d3dPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dPP.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dPP.BackBufferCount = 1;
    d3dPP.BackBufferWidth = width;
    d3dPP.BackBufferHeight = height;
    d3dPP.hDeviceWindow = hWnd;

    HRESULT hr = d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                    D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dPP, &d3dDevice);
    if (FAILED(hr))
        return false;

    hr = D3DXCreateSprite(d3dDevice, &spriteBrush);
    return SUCCEEDED(hr);
}

void Graphics::beginFrame()
{
    d3dDevice->Clear(
        0, NULL, D3DCLEAR_TARGET,
        D3DCOLOR_XRGB(222, 216, 200),   // warm sky, fills layer gaps
        1.0f, 0);

    d3dDevice->BeginScene();
    spriteBrush->Begin(D3DXSPRITE_ALPHABLEND);
}

void Graphics::endFrame()
{
    spriteBrush->End();
    d3dDevice->EndScene();
    d3dDevice->Present(NULL, NULL, NULL, NULL);
}

void Graphics::release()
{
    if (spriteBrush != NULL) { spriteBrush->Release(); spriteBrush = NULL; }
    if (d3dDevice != NULL)   { d3dDevice->Release();   d3dDevice = NULL; }
    if (d3d9 != NULL)        { d3d9->Release();        d3d9 = NULL; }
}
