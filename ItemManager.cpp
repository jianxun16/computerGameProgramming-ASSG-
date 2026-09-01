#include "ItemManager.h"
#include "PlayerAnimation.h"
#include "TileMap.h"

ItemManager::ItemManager()
{
    mushroomTex = NULL;
    tomatoTex = NULL;
}

ItemManager::~ItemManager()
{
    release();
}

void ItemManager::add(Type type, float worldX, float worldY)
{
    Item it;
    it.type = type;
    it.x = worldX;
    it.y = worldY;
    it.active = true;
    items.push_back(it);
}

bool ItemManager::load(IDirect3DDevice9* device)
{
    D3DXCreateTextureFromFile(device, "Assets/item/mushroom.png", &mushroomTex);
    D3DXCreateTextureFromFile(device, "Assets/item/tomato.png", &tomatoTex);

    // Sit each item on top of the ground (floor top is row 7 = 448).
    float onFloor = 7 * TileMap::TILE - SIZE;   // 384

    add(TOMATO,   11 * TileMap::TILE, onFloor);   
    add(MUSHROOM, 20 * TileMap::TILE, onFloor);   

    return true;
}

void ItemManager::update(PlayerAnimation* player)
{
    float pl, pt, pr, pb;
    player->getWorldHitbox(pl, pt, pr, pb);

    for (size_t i = 0; i < items.size(); i++)
    {
        Item& it = items[i];
        if (!it.active)
            continue;

        // AABB overlap between the player and this item.
        bool hit = !(pr < it.x || pl > it.x + SIZE || pb < it.y || pt > it.y + SIZE);
        if (!hit)
            continue;

        it.active = false;   // eaten

        float s = player->getScale();
        if (it.type == MUSHROOM)
        {
            s -= 0.5f;
            if (s < 1.0f) s = 1.0f;      // don't shrink below original
        }
        else // TOMATO
        {
            s += 0.5f;
            if (s > 4.0f) s = 4.0f;      // cap how big it gets
        }
        player->setScale(s);
    }
}

void ItemManager::render(LPD3DXSPRITE spriteBrush, float cameraX)
{
    for (size_t i = 0; i < items.size(); i++)
    {
        Item& it = items[i];
        if (!it.active)
            continue;

        D3DXVECTOR3 pos(it.x - cameraX, it.y, 0.0f);
        LPDIRECT3DTEXTURE9 tex = (it.type == MUSHROOM) ? mushroomTex : tomatoTex;
        spriteBrush->Draw(tex, NULL, NULL, &pos, D3DCOLOR_XRGB(255, 255, 255));
    }
}

void ItemManager::reset()
{
    for (size_t i = 0; i < items.size(); i++)
        items[i].active = true;
}

void ItemManager::release()
{
    if (mushroomTex != NULL) { mushroomTex->Release(); mushroomTex = NULL; }
    if (tomatoTex != NULL)   { tomatoTex->Release();   tomatoTex = NULL; }
}
