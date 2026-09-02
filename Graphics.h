#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <string>

using namespace std;

class Graphics {
private:
    IDirect3DDevice9* device;
    LPD3DXSPRITE spriteBrush;
    LPD3DXLINE lineBrush;
    LPD3DXFONT fontBrush;

public:
    bool InitializeGraphics(IDirect3DDevice9* d3dDevice);
    void CleanUpGraphics();

    LPDIRECT3DTEXTURE9 LoadTexture(string path);

    void BeginRender(int r, int g, int b);

    void DrawSprite(LPDIRECT3DTEXTURE9 texture, RECT* srcRect, D3DXMATRIX* transform,
        D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255));

    void DrawString(const string& text, RECT rect, D3DCOLOR color,
        DWORD format = DT_LEFT | DT_TOP | DT_SINGLELINE);

    void DrawLine(D3DXVECTOR2 from, D3DXVECTOR2 to, float width, D3DCOLOR color);

    void EndRender();
};