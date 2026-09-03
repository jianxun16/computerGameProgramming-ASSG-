#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include "Graphics.h"
#include "Camera.h"

// A small, reusable renderer for a single textured quad.
//
// A Sprite knows *how* to draw one texture: which sub-rectangle of a sheet to
// show, where its pivot sits, and how to build the world/screen transform. It
// hides the D3DX matrix maths so callers just say "draw this here, this big".
//
// Ownership: a Sprite only *borrows* the texture pointer. Whoever loaded the
// texture (the object itself, or the AssetManager / Graphics wrapper) is still
// responsible for releasing it. A Sprite never Release()es the texture, so it
// is safe to copy and to share one texture between many Sprites.
class Sprite {
private:
    LPDIRECT3DTEXTURE9 texture;   // borrowed, not owned
    RECT  sourceRect;             // sub-rectangle of the sheet to draw
    bool  useSourceRect;          // false -> draw the whole texture
    D3DXVECTOR2 origin;           // pivot in source pixels, before scaling

    // Shared matrix builder used by both the world- and screen-space draws.
    D3DXMATRIX BuildTransform(D3DXVECTOR2 position, D3DXVECTOR2 scale, float rotation);

public:
    Sprite();
    explicit Sprite(LPDIRECT3DTEXTURE9 tex);

    void SetTexture(LPDIRECT3DTEXTURE9 tex) { texture = tex; }
    LPDIRECT3DTEXTURE9 GetTexture() const { return texture; }
    bool IsValid() const { return texture != NULL; }

    // Sprite-sheet support: choose the one cell / animation frame to draw.
    void SetSourceRect(const RECT& rect) { sourceRect = rect; useSourceRect = true; }
    void ClearSourceRect() { useSourceRect = false; }

    // Pivot the quad scales / rotates around, in source pixels. (0,0) is the
    // top-left corner (the D3DXSPRITE default); CenterOrigin() puts it in the
    // middle of the current frame so flips and rotations stay in place.
    void SetOrigin(float x, float y) { origin = D3DXVECTOR2(x, y); }
    void CenterOrigin();

    // World-space draw: the camera scrolls the sprite with the level. A
    // negative scale.x mirrors the sprite (used for left/right facing).
    void Draw(Graphics* graphics, Camera* camera,
        D3DXVECTOR2 position,
        D3DXVECTOR2 scale = D3DXVECTOR2(1.0f, 1.0f),
        float rotation = 0.0f,
        D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255));

    // Screen-space draw: fixed on screen, ignores the camera. Use it for UI,
    // menus and the parallax background layers.
    void DrawScreen(Graphics* graphics,
        D3DXVECTOR2 position,
        D3DXVECTOR2 scale = D3DXVECTOR2(1.0f, 1.0f),
        float rotation = 0.0f,
        D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255));
};
