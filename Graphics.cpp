#include "Graphics.h"
#include <iostream>

bool Graphics::InitializeGraphics(IDirect3DDevice9* d3dDevice) {
    device = d3dDevice;

    if (FAILED(D3DXCreateSprite(device, &spriteBrush))) {
        cout << "Failed to create sprite brush." << endl;
        return false;
    }

    if (FAILED(D3DXCreateLine(device, &lineBrush))) {
        cout << "Failed to create line brush." << endl;
        return false;
    }

    if (FAILED(D3DXCreateFont(device, 25, 0, 0, 1, false, DEFAULT_CHARSET,
        OUT_TT_ONLY_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial", &fontBrush))) {
        cout << "Failed to create font." << endl;
        return false;
    }

    return true;
}

LPDIRECT3DTEXTURE9 Graphics::LoadTexture(string path) {
    LPDIRECT3DTEXTURE9 texture = NULL;

    D3DXCreateTextureFromFileEx(device, path.c_str(), D3DX_DEFAULT, D3DX_DEFAULT,
        D3DX_DEFAULT, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT, 0,
        NULL, NULL, &texture);

    return texture;
}

void Graphics::BeginRender(int r, int g, int b) {
    device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(r, g, b), 1.0f, 0);
    device->BeginScene();
    spriteBrush->Begin(D3DXSPRITE_ALPHABLEND);
}

void Graphics::DrawSprite(LPDIRECT3DTEXTURE9 texture, RECT* srcRect, D3DXMATRIX* transform, D3DCOLOR color) {
    if (transform) {
        spriteBrush->SetTransform(transform);
    }
    spriteBrush->Draw(texture, srcRect, NULL, NULL, color);
}

void Graphics::DrawString(const string& text, RECT rect, D3DCOLOR color, DWORD format) {
    if (!fontBrush) return;
    fontBrush->DrawTextA(spriteBrush, text.c_str(), -1, &rect, format, color);
}

void Graphics::DrawLine(D3DXVECTOR2 from, D3DXVECTOR2 to, float width, D3DCOLOR color) {
    if (!lineBrush) return;
    D3DXVECTOR2 pts[2] = { from, to };
    lineBrush->SetWidth(width);
    lineBrush->Begin();          // line brush has its own batch
    lineBrush->Draw(pts, 2, color);
    lineBrush->End();
}

void Graphics::EndRender() {
    spriteBrush->End();
    device->EndScene();
    device->Present(NULL, NULL, NULL, NULL);
}

void Graphics::CleanUpGraphics() {
    if (spriteBrush) { spriteBrush->Release(); spriteBrush = NULL; }
    if (lineBrush) { lineBrush->Release();   lineBrush = NULL; }
    if (fontBrush) { fontBrush->Release();   fontBrush = NULL; }
}