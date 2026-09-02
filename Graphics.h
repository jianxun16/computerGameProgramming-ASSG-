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

    bool LoadFont(string fontName, int size, int weight);

    void BeginRender(int r, int g, int b);

    void DrawSprite(LPDIRECT3DTEXTURE9 texture, RECT* srcRect, D3DXMATRIX* transform, D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255));

    void DrawString(string text, RECT* rect, DWORD alignment, D3DCOLOR color);

    void DrawLine(D3DXVECTOR2 from, D3DXVECTOR2 to, float width, D3DCOLOR color);

    void EndRender();
};