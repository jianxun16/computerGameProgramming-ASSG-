#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include "Graphics.h"
#include "Camera.h"
#include "Sprite.h"


class TileMap
{
public:
    static const int TILE = 64;
    static const int ROWS = 9;
    static const int COLS = 28;

    
    enum { EMPTY = 0, GRASS = 1, ROCK = 2, SPIKE = 5 };

    TileMap();
    ~TileMap();
   
    bool load(Graphics* graphics, const char* mapFile = "Assets/Map/Map1.txt");
    
    void render(Graphics* graphics, Camera* camera, int screenWidth);
    void release();

    bool  rectSolid(float left, float top, float right, float bottom) const;
    bool  rectSpike(float left, float top, float right, float bottom) const; 
    float groundTopYAt(float worldX) const;  

private:
    bool solid(int col, int row) const;

    int tiles[ROWS][COLS];
    LPDIRECT3DTEXTURE9 grassTex;
    LPDIRECT3DTEXTURE9 rockTex;
    LPDIRECT3DTEXTURE9 spikeTex;

    Sprite grassSprite;   // reusable renderers, one per tile texture
    Sprite rockSprite;
    Sprite spikeSprite;
};