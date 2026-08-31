#include "TileMap.h"
#include <math.h>
#include <fstream>
using namespace std;

TileMap::TileMap()
{
    grassTex = NULL;
    rockTex = NULL;
    spikeTex = NULL;
    ZeroMemory(tiles, sizeof(tiles));
}

TileMap::~TileMap()
{
    release();
}

bool TileMap::load(IDirect3DDevice9* device, const char* mapFile)
{
    D3DXCreateTextureFromFile(device, "Assets/mapItem/grassBlock.png", &grassTex);
    D3DXCreateTextureFromFile(device, "Assets/mapItem/rock.png", &rockTex);
    D3DXCreateTextureFromFile(device, "Assets/mapItem/spike.png", &spikeTex);

    // The level. 0 = empty, 1 = grass, 2 = rock.  (28 wide, 9 tall)
    //  - rows 7-8 are the ground


    ifstream file(mapFile);
    if (!file) { MessageBox(NULL, mapFile, "map file not found!", MB_OK); return false; }

    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
        {
            char ch;
            file >> ch;
            tiles[r][c] = ch - '0';
        }
    return true;
}

bool TileMap::solid(int col, int row) const
{
    if (col < 0 || col >= COLS || row < 0 || row >= ROWS)
        return false;   // outside the map = open air
    // Spikes are NOT solid: you walk INTO them (and die), you don't stand on them.
    return tiles[row][col] != EMPTY && tiles[row][col] != SPIKE;
}

bool TileMap::rectSpike(float left, float top, float right, float bottom) const
{
    int c0 = (int)floorf(left / TILE);
    int c1 = (int)floorf((right - 0.001f) / TILE);
    int r0 = (int)floorf(top / TILE);
    int r1 = (int)floorf((bottom - 0.001f) / TILE);

    for (int r = r0; r <= r1; r++)
        for (int c = c0; c <= c1; c++)
        {
            if (c < 0 || c >= COLS || r < 0 || r >= ROWS) continue;
            if (tiles[r][c] != SPIKE) continue;

            // TRIANGLE collision. The spike is a triangle: apex at the top-centre
            // of the cell (width 0) widening to the base (full width). So only a
            // real overlap with that triangle counts - the empty top corners of
            // the cell do NOT kill you.
            float tileTop = (float)(r * TILE);
            float centreX = (float)(c * TILE) + TILE * 0.5f;

            // Test at the deepest point the player reaches in this cell, where the
            // triangle is widest (best chance of a hit).
            float y = bottom;
            if (y > tileTop + (float)TILE) y = tileTop + (float)TILE;   // clamp to base
            float depth = y - tileTop;                 // 0 at the apex .. TILE at the base
            if (depth <= 0.0f) continue;               // player is above the tip

            float halfW = depth * 0.5f;                // TILE*0.5 at the base (0.5 = base slope)
            if (right > centreX - halfW && left < centreX + halfW)
                return true;
        }
    return false;
}

bool TileMap::rectSolid(float left, float top, float right, float bottom) const
{
    int c0 = (int)floorf(left / TILE);
    int c1 = (int)floorf((right - 0.001f) / TILE);
    int r0 = (int)floorf(top / TILE);
    int r1 = (int)floorf((bottom - 0.001f) / TILE);

    for (int r = r0; r <= r1; r++)
        for (int c = c0; c <= c1; c++)
            if (solid(c, r))
                return true;
    return false;
}

float TileMap::groundTopYAt(float worldX) const
{
    int col = (int)floorf(worldX / TILE);
    for (int r = 0; r < ROWS; r++)
        if (solid(col, r))
            return (float)(r * TILE);
    return (float)(ROWS * TILE);   // no ground -> bottom of the map
}

void TileMap::render(LPD3DXSPRITE spriteBrush, float cameraX, int screenWidth)
{
    // Only draw the columns that are on screen.
    int firstCol = (int)(cameraX / TILE);
    int lastCol = (int)((cameraX + screenWidth) / TILE) + 1;
    if (firstCol < 0) firstCol = 0;
    if (lastCol > COLS) lastCol = COLS;

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = firstCol; c < lastCol; c++)
        {
            int t = tiles[r][c];
            if (t == EMPTY)
                continue;

            float sx = c * (float)TILE - cameraX;
            float sy = (float)(r * TILE);

            if (t == GRASS)
            {
                D3DXVECTOR3 pos(sx, sy, 0.0f);
                spriteBrush->Draw(grassTex, NULL, NULL, &pos, D3DCOLOR_XRGB(255, 255, 255));
            }
            else if (t == SPIKE)
            {
                D3DXVECTOR3 pos(sx, sy, 0.0f);
                spriteBrush->Draw(spikeTex, NULL, NULL, &pos, D3DCOLOR_XRGB(255, 255, 255));
            }
            else // ROCK (32x32 art scaled up to fill the 64 cell)
            {
                D3DXMATRIX scaleM, transM, world, identity;
                D3DXMatrixScaling(&scaleM, 2.0f, 2.0f, 1.0f);
                D3DXMatrixTranslation(&transM, sx, sy, 0.0f);
                world = scaleM * transM;
                spriteBrush->SetTransform(&world);

                D3DXVECTOR3 zero(0.0f, 0.0f, 0.0f);
                spriteBrush->Draw(rockTex, NULL, NULL, &zero, D3DCOLOR_XRGB(255, 255, 255));

                D3DXMatrixIdentity(&identity);
                spriteBrush->SetTransform(&identity);
            }
        }
    }
}

void TileMap::release()
{
    if (grassTex != NULL) { grassTex->Release(); grassTex = NULL; }
    if (rockTex != NULL)  { rockTex->Release();  rockTex = NULL; }
    if (spikeTex != NULL) { spikeTex->Release(); spikeTex = NULL; }
}
