#pragma once
#include "Graphics.h"
#include "Camera.h"

// parallex
class Background
{
public:
    static const int NUM_LAYERS = 4; 

    Background();
    ~Background();

    bool load(Graphics* graphics);

    void render(Graphics* graphics, Camera* camera);

    void release();

private:
    void drawLayer(Graphics* graphics, float cameraX, int i);

    LPDIRECT3DTEXTURE9 layerTexture[NUM_LAYERS]; 
    float parallaxFactor[NUM_LAYERS]; 
    int sourceSize;  
    float scale;        
};