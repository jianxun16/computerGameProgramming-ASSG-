#pragma once
#include <d3d9.h>
#include <d3dx9.h>

// Scrolling multi-layer parallax forest. Layer 0 farthest/slowest, layer 3
// nearest. Seamless looping uses only fmodf (no if / for / while).
class Background
{
public:
    static const int NUM_LAYERS = 4;

    Background();
    ~Background();

    // Load the forest layers (back -> front).
    bool load(IDirect3DDevice9* device);

    // Scroll by the character's horizontal movement this step.
    void update(float deltaX);

    // Draw every layer, between spriteBrush->Begin() and ->End().
    void render(LPD3DXSPRITE spriteBrush, int screenWidth);

    // Free the textures.
    void release();

private:
    // Draw one layer as 3 looping tiles.
    void drawLayer(LPD3DXSPRITE spriteBrush, int i);

    LPDIRECT3DTEXTURE9 layerTexture[NUM_LAYERS];
    float              parallaxFactor[NUM_LAYERS];  // scroll speed per layer

    int   sourceSize;   // PNG size (512)
    float scale;        // uniform scale
    float cameraX;      // accumulated scroll
};
