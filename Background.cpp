#include "Background.h"
#include <math.h>

Background::Background()
{
    sourceSize = 512;
    scale = 1.85f;


    parallaxFactor[0] = 0.10f;   
    parallaxFactor[1] = 0.40f;
    parallaxFactor[2] = 0.80f;
    parallaxFactor[3] = 1.00f;  

    layerTexture[0] = NULL; 
    layerTexture[1] = NULL;
    layerTexture[2] = NULL;
    layerTexture[3] = NULL; 
}

Background::~Background()
{
    release();
}

bool Background::load(Graphics* graphics)
{
    layerTexture[0] = graphics->LoadTexture("parallex/parallaxForest4.png"); // farthest
    layerTexture[1] = graphics->LoadTexture("parallex/parallaxForest3.png");
    layerTexture[2] = graphics->LoadTexture("parallex/parallaxForest2.png");
    layerTexture[3] = graphics->LoadTexture("parallex/parallaxForest1a.png"); // nearest
    return true;
}

void Background::drawLayer(Graphics* graphics, float cameraX, int i)
{
    float tileW = (float)sourceSize; 

    float scroll = (cameraX * parallaxFactor[i]) / scale;

    float wrapped = fmodf(fmodf(scroll, tileW) + tileW, tileW);

    float base = -wrapped;

    D3DXMATRIX scaleMat;
    D3DXMatrixScaling(&scaleMat, scale, scale, 1.0f);


    D3DXMATRIX transA, transB, transC;
    D3DXMatrixTranslation(&transA, base * scale, 0.0f, 0.0f);
    D3DXMatrixTranslation(&transB, (base + tileW) * scale, 0.0f, 0.0f);
    D3DXMatrixTranslation(&transC, (base + tileW * 2.0f) * scale, 0.0f, 0.0f);

    D3DXMATRIX matA = scaleMat * transA;
    D3DXMATRIX matB = scaleMat * transB;
    D3DXMATRIX matC = scaleMat * transC;

    graphics->DrawSprite(layerTexture[i], NULL, &matA);
    graphics->DrawSprite(layerTexture[i], NULL, &matB);
    graphics->DrawSprite(layerTexture[i], NULL, &matC);
}

void Background::render(Graphics* graphics, Camera* camera)
{
    // get cam pos
    float currentCamX = camera->GetPosition().x;

    drawLayer(graphics, currentCamX, 0);
    drawLayer(graphics, currentCamX, 1);
    drawLayer(graphics, currentCamX, 2);
    drawLayer(graphics, currentCamX, 3);
}

void Background::release()
{
    if (layerTexture[0]) { layerTexture[0]->Release(); layerTexture[0] = NULL; }
    if (layerTexture[1]) { layerTexture[1]->Release(); layerTexture[1] = NULL; } 
    if (layerTexture[2]) { layerTexture[2]->Release(); layerTexture[2] = NULL; } 
    if (layerTexture[3]) { layerTexture[3]->Release(); layerTexture[3] = NULL; }
}