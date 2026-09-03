#pragma once
#include "Graphics.h"
#include "Camera.h"
#include <vector>

class Player;

// Collectable items in world coordinates, scrolling with the tile map.
// mushroom -> shrink the player, tomato -> grow the player.
class ItemManager {
public:
    enum Type { MUSHROOM, TOMATO };

    ItemManager();

    bool load(Graphics* graphics);
    void update(Player* player);                     // pick up on overlap
    void render(Graphics* graphics, Camera* camera); // draw scrolled with the world
    void reset();                                    // bring all items back

private:
    struct Item { Type type; float x; float y; bool active; };
    void add(Type type, float worldX, float worldY);

    static const int SIZE = 64;

    std::vector<Item>  items;
    LPDIRECT3DTEXTURE9 mushroomTex;
    LPDIRECT3DTEXTURE9 tomatoTex;
};
