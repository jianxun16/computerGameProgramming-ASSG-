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
    void DrawSprite(LPDIRECT3DTEXTURE9 texture, RECT* srcRect, D3DXMATRIX* transform);
    void EndRender();
};