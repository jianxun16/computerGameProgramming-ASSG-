#pragma once
#include <d3d9.h>
#include <d3dx9.h>

// A finite grid floor built from 64x64 tiles.
//   0 = empty, 1 = grass (solid), 2 = rock (solid, different sprite)
// The map lives in world coordinates; render() scrolls it by cameraX so it
// lines up with the parallax background. Collision is plain AABB-vs-tiles.
class TileMap
{
public:
    static const int TILE = 64;
    static const int ROWS = 9;
    static const int COLS = 28;

    //   0 empty  1 grass  2 rock  5 spike (deadly, NOT solid - walk into it = die)
    enum { EMPTY = 0, GRASS = 1, ROCK = 2, SPIKE = 5 };

    TileMap();
    ~TileMap();

    // mapFile lets each room load its own layout (PlayState = Map1, BossState = Map2).
    bool load(IDirect3DDevice9* device, const char* mapFile = "Assets/Map/Map1.txt");
    void render(LPD3DXSPRITE spriteBrush, float cameraX, int screenWidth);
    void release();

    // --- Collision ---
    // Is any solid tile touched by this world-space rectangle?
    bool  rectSolid(float left, float top, float right, float bottom) const;
    // Is any SPIKE tile touched by this world-space rectangle? (-> death)
    bool  rectSpike(float left, float top, float right, float bottom) const;
    // Top Y of the highest solid tile in a world column (for spawning / ground).
    float groundTopYAt(float worldX) const;

private:
    bool solid(int col, int row) const;

    int tiles[ROWS][COLS];
    LPDIRECT3DTEXTURE9 grassTex;
    LPDIRECT3DTEXTURE9 rockTex;
    LPDIRECT3DTEXTURE9 spikeTex;
};