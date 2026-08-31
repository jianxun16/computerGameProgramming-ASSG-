#pragma once
#include <d3d9.h>
#include <d3dx9.h>

// A scrolling multi-layer parallax forest background.
// Layer 0 is the farthest (scrolls slowest); layer 3 is the nearest.
// The seamless looping uses only maths (fmodf) - no if / for / while.
class Background
{
public:
    static const int NUM_LAYERS = 4;

    Background();
    ~Background();

    // Load the forest layers (back -> front).
    bool load(IDirect3DDevice9* device);

    // Scroll the background. deltaX is the character's horizontal movement this step.
    void update(float deltaX);

    // Draw every layer. Call between spriteBrush->Begin() and spriteBrush->End().
    void render(LPD3DXSPRITE spriteBrush, int screenWidth);

    // Free the textures.
    void release();

private:
    // Draw one layer as 3 looping tiles (a, b, c) - no if / for / while.
    void drawLayer(LPD3DXSPRITE spriteBrush, int i);

    LPDIRECT3DTEXTURE9 layerTexture[NUM_LAYERS];
    float              parallaxFactor[NUM_LAYERS];  // how fast each layer scrolls

    int   sourceSize;   // each PNG is sourceSize x sourceSize (512)
    float scale;        // uniform scale applied to every layer
    float cameraX;      // total horizontal scroll accumulated from movement
};
