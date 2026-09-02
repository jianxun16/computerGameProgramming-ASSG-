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

bool TileMap::load(Graphics* graphics, const char* mapFile)
{
    grassTex = graphics->LoadTexture("Assets/mapItem/grassBlock.png");
    rockTex = graphics->LoadTexture("Assets/mapItem/rock.png");
    spikeTex = graphics->LoadTexture("Assets/mapItem/spike.png");

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
        return false;   
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

                        float tileTop = (float)(r * TILE); 
                        float centreX = (float)(c * TILE) + TILE * 0.5f; 

                        float y = bottom;
                        if (y > tileTop + (float)TILE) y = tileTop + (float)TILE;  
                float depth = y - tileTop;                
                if (depth <= 0.0f) continue;              

                float halfW = depth * 0.5f;               
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
                return (float)(ROWS * TILE);   
}

void TileMap::render(Graphics* graphics, Camera* camera, int screenWidth)
{
    float cameraX = camera->GetPosition().x;

    int firstCol = (int)(cameraX / TILE);
    int lastCol = (int)((cameraX + screenWidth) / TILE) + 1;
    if (firstCol < 0) firstCol = 0;
    if (lastCol > COLS) lastCol = COLS;

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = firstCol; c < lastCol; c++)
        {
            int t = tiles[r][c];
            if (t == EMPTY) continue;

            float worldX = c * (float)TILE;
            float worldY = r * (float)TILE;

            D3DXMATRIX scaleMat, transMat, worldMat, finalMatrix;

            if (t == ROCK) {
                D3DXMatrixScaling(&scaleMat, 2.0f, 2.0f, 1.0f);
            }
            else {
                D3DXMatrixScaling(&scaleMat, 1.0f, 1.0f, 1.0f);
            }

            D3DXMatrixTranslation(&transMat, worldX, worldY, 0.0f);
            worldMat = scaleMat * transMat;

            finalMatrix = worldMat * camera->GetViewMatrix();

            if (t == GRASS) {
                graphics->DrawSprite(grassTex, NULL, &finalMatrix);
            }
            else if (t == SPIKE) {
                graphics->DrawSprite(spikeTex, NULL, &finalMatrix);
            }
            else if (t == ROCK) {
                graphics->DrawSprite(rockTex, NULL, &finalMatrix);
            }
        }
    }
}

void TileMap::release()
{
    if (grassTex != NULL) { grassTex->Release(); grassTex = NULL; }
    if (rockTex != NULL) { rockTex->Release();  rockTex = NULL; }
    if (spikeTex != NULL) { spikeTex->Release(); spikeTex = NULL; }
}