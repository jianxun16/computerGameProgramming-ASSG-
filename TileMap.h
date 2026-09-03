#pragma once
#include <d3d9.h>
#include <d3dx9.h>

// Finite 64x64-tile grid in world coordinates; render() scrolls it by cameraX
// to line up with the parallax background. Collision is AABB-vs-tiles.
class TileMap
{
public:
    static const int TILE = 64;
    static const int ROWS = 9;
    static const int COLS = 28;

    // 0 empty, 1 grass, 2 rock, 5 spike (deadly, not solid).
    enum { EMPTY = 0, GRASS = 1, ROCK = 2, SPIKE = 5 };

    TileMap();
    ~TileMap();

    // mapFile picks each room's layout (PlayState = Map1, BossState = Map2).
    bool load(IDirect3DDevice9* device, const char* mapFile = "Assets/Map/Map1.txt");
    void render(LPD3DXSPRITE spriteBrush, float cameraX, int screenWidth);
    void release();

    // --- Collision (world-space rectangle) ---
    bool  rectSolid(float left, float top, float right, float bottom) const;  // hits a solid tile?
    bool  rectSpike(float left, float top, float right, float bottom) const;  // hits a spike? (death)
    float groundTopYAt(float worldX) const;   // top Y of the highest solid tile in a column

private:
    bool solid(int col, int row) const;

    int tiles[ROWS][COLS];
    LPDIRECT3DTEXTURE9 grassTex;
    LPDIRECT3DTEXTURE9 rockTex;
    LPDIRECT3DTEXTURE9 spikeTex;
};