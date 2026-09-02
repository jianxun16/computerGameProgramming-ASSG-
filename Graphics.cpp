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

    if (FAILED(device->CreateTexture(1, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &whiteTex, NULL))) {
        cout << "Failed to create UI white texture." << endl;
        return false;
    }
    D3DLOCKED_RECT lr;
    whiteTex->LockRect(0, &lr, NULL, 0);
    *(DWORD*)lr.pBits = 0xFFFFFFFF;
    whiteTex->UnlockRect(0);

    return true;
}

LPDIRECT3DTEXTURE9 Graphics::LoadTexture(string path) {
    LPDIRECT3DTEXTURE9 texture = NULL;

    if (FAILED(D3DXCreateTextureFromFileEx(device, path.c_str(), D3DX_DEFAULT, D3DX_DEFAULT,
        D3DX_DEFAULT, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT, 0,
        NULL, NULL, &texture))) {
        cout << "Failed to load texture: " << path << endl;
    };

    return texture;
}

bool Graphics::LoadFont(string fontName, int size, int weight) {
    HRESULT hr = D3DXCreateFont(device, size, 0, weight, 1, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        fontName.c_str(), &fontBrush);

    return SUCCEEDED(hr);
}

void Graphics::BeginRender(int r, int g, int b) {
    device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(r, g, b), 1.0f, 0);
    device->BeginScene();
    spriteBrush->Begin(D3DXSPRITE_ALPHABLEND);
}

void Graphics::DrawSprite(LPDIRECT3DTEXTURE9 texture, RECT* srcRect, D3DXMATRIX* transform, D3DCOLOR color) {
    if (spriteBrush && texture) {
        spriteBrush->SetTransform(transform);
        spriteBrush->Draw(texture, srcRect, NULL, NULL, color);
    }
}

void Graphics::DrawString(string text, RECT* rect, DWORD alignment, D3DCOLOR color) {
    if (fontBrush && spriteBrush) {
        D3DXMATRIX identity;
        D3DXMatrixIdentity(&identity);
        spriteBrush->SetTransform(&identity);

        fontBrush->DrawTextA(spriteBrush, text.c_str(), -1, rect, alignment, color);
    }
}

void Graphics::DrawLine(D3DXVECTOR2 from, D3DXVECTOR2 to, float width, D3DCOLOR color) {
    if (!lineBrush) return;
    D3DXVECTOR2 pts[2] = { from, to };
    lineBrush->SetWidth(width);
    lineBrush->Begin();          
    lineBrush->Draw(pts, 2, color);
    lineBrush->End();
}

void Graphics::DrawRect(float x, float y, float w, float h, D3DCOLOR color) {
    if (!spriteBrush || !whiteTex) return;

    D3DXVECTOR2 scaling(w, h);         
    D3DXVECTOR2 trans(x, y);

    D3DXMATRIX m;
    D3DXMatrixTransformation2D(&m, NULL, 0.0f, &scaling, NULL, 0.0f, &trans);
    spriteBrush->SetTransform(&m);

    D3DXVECTOR3 origin(0.0f, 0.0f, 0.0f);
    spriteBrush->Draw(whiteTex, NULL, NULL, &origin, color);
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