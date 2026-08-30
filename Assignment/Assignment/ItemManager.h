#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>

class PlayerAnimation;

// Collectable items placed in the world.
//   mushroom -> shrink the player   tomato -> grow the player
// Items live in world coordinates and scroll with the tile map.
class ItemManager
{
public:
    enum Type { MUSHROOM, TOMATO };

    ItemManager();
    ~ItemManager();

    bool load(IDirect3DDevice9* device);
    void update(PlayerAnimation* player);            // pick up on overlap
    void render(LPD3DXSPRITE spriteBrush, float cameraX);
    void reset();                                    // bring all items back
    void release();

private:
    struct Item { Type type; float x; float y; bool active; };
    void add(Type type, float worldX, float worldY);

    static const int SIZE = 64;

    std::vector<Item>  items;
    LPDIRECT3DTEXTURE9 mushroomTex;
    LPDIRECT3DTEXTURE9 tomatoTex;
};
