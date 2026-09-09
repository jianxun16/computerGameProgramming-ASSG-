#pragma once
#include "Graphics.h"
#include "Camera.h"
#include "Sprite.h"

// parallex
class Background
{
public:
    static const int NUM_LAYERS = 4; 

    Background();
    ~Background();

    bool Load(Graphics* graphics);

    void Render(Graphics* graphics, Camera* camera);

    void Release();

private:
    void DrawLayer(Graphics* graphics, float cameraX, int i);

    LPDIRECT3DTEXTURE9 layerTexture[NUM_LAYERS];
    float parallaxFactor[NUM_LAYERS];
    int sourceSize;
    float scale;

    Sprite layerSprite;   // reusable renderer, re-pointed at each layer texture
};