#include "Background.h"
#include <math.h>

Background::Background()
{
    sourceSize = 512;
    scale = 1.85f;      // scale so the forest fills the window
    cameraX = 0.0f;

    // Farther layers scroll slower, nearer layers scroll faster.
    // Layer 3 is nearest (factor 1.0), so the ground scrolls exactly with steps.
    parallaxFactor[0] = 0.10f;   // farthest
    parallaxFactor[1] = 0.40f;
    parallaxFactor[2] = 0.80f;
    parallaxFactor[3] = 1.00f;   // nearest

    layerTexture[0] = NULL;
    layerTexture[1] = NULL;
    layerTexture[2] = NULL;
    layerTexture[3] = NULL;
}

Background::~Background()
{
    release();
}

bool Background::load(IDirect3DDevice9* device)
{
    // Back -> front (farthest layer first). No loop - each layer loaded explicitly.
    D3DXCreateTextureFromFile(device, "parallex/parallaxForest4.png", &layerTexture[0]); // farthest
    D3DXCreateTextureFromFile(device, "parallex/parallaxForest3.png", &layerTexture[1]);
    D3DXCreateTextureFromFile(device, "parallex/parallaxForest2.png", &layerTexture[2]);
    D3DXCreateTextureFromFile(device, "parallex/parallaxForest1a.png", &layerTexture[3]); // nearest
    return true;
}

void Background::update(float deltaX)
{
    // Just accumulate the movement; the looping maths happens in render().
    cameraX += deltaX;
}

void Background::drawLayer(LPD3DXSPRITE spriteBrush, int i)
{
    float tileW = (float)sourceSize;   // one tile is 512 wide (pre-scale)

    // How far this layer has scrolled, in pre-scale space.
    float scroll = (cameraX * parallaxFactor[i]) / scale;

    // Wrap the scroll into [0, tileW) using only maths (no if):
    //   fmodf can be negative, so add tileW and fmodf again -> always positive.
    float wrapped = fmodf(fmodf(scroll, tileW) + tileW, tileW);

    // Left-most tile always sits in (-tileW, 0], so 3 tiles cover the screen.
    float base = -wrapped;

    // The three looping copies a, b, c - drawn explicitly, no for loop.
    D3DXVECTOR3 a(base, 0.0f, 0.0f);
    D3DXVECTOR3 b(base + tileW, 0.0f, 0.0f);
    D3DXVECTOR3 c(base + tileW * 2.0f, 0.0f, 0.0f);

    spriteBrush->Draw(layerTexture[i], NULL, NULL, &a, D3DCOLOR_XRGB(255, 255, 255));
    spriteBrush->Draw(layerTexture[i], NULL, NULL, &b, D3DCOLOR_XRGB(255, 255, 255));
    spriteBrush->Draw(layerTexture[i], NULL, NULL, &c, D3DCOLOR_XRGB(255, 255, 255));
}

void Background::render(LPD3DXSPRITE spriteBrush, int screenWidth)
{
    // One scale transform enlarges every layer; tile positions are D3DXVECTOR3
    // in pre-scale space (screen x = scale * position.x).
    D3DXMATRIX matScale;
    D3DXMatrixScaling(&matScale, scale, scale, 1.0f);
    spriteBrush->SetTransform(&matScale);

    // Draw back -> front. Each layer drawn explicitly, no for loop.
    drawLayer(spriteBrush, 0);
    drawLayer(spriteBrush, 1);
    drawLayer(spriteBrush, 2);
    drawLayer(spriteBrush, 3);

    // Reset the transform so whatever is drawn next is at normal scale/position.
    D3DXMATRIX identity;
    D3DXMatrixIdentity(&identity);
    spriteBrush->SetTransform(&identity);
}

void Background::release()
{
    if (layerTexture[0]) { layerTexture[0]->Release(); layerTexture[0] = NULL; }
    if (layerTexture[1]) { layerTexture[1]->Release(); layerTexture[1] = NULL; }
    if (layerTexture[2]) { layerTexture[2]->Release(); layerTexture[2] = NULL; }
    if (layerTexture[3]) { layerTexture[3]->Release(); layerTexture[3] = NULL; }
}
