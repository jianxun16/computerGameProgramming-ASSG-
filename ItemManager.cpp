#include "ItemManager.h"
#include "Player.h"
#include "TileMap.h"

ItemManager::ItemManager() {
    mushroomTex = NULL;
    tomatoTex = NULL;
}

void ItemManager::add(Type type, float worldX, float worldY) {
    Item it;
    it.type = type;
    it.x = worldX;
    it.y = worldY;
    it.active = true;
    items.push_back(it);
}

bool ItemManager::load(Graphics* graphics) {
    mushroomTex = graphics->LoadTexture("Assets/item/mushroom.png");
    tomatoTex = graphics->LoadTexture("Assets/item/tomato.png");

    mushroomSprite.SetTexture(mushroomTex);
    tomatoSprite.SetTexture(tomatoTex);

    // Sit each item on the ground (floor top = row 7).
    float onFloor = 7.0f * TileMap::TILE - SIZE;

    add(TOMATO, 11.0f * TileMap::TILE, onFloor);
    add(MUSHROOM, 20.0f * TileMap::TILE, onFloor);

    return true;
}

void ItemManager::update(Player* player) {
    float pl, pt, pr, pb;
    player->GetWorldHitbox(pl, pt, pr, pb);

    for (size_t i = 0; i < items.size(); i++) {
        Item& it = items[i];
        if (!it.active)
            continue;

        // AABB overlap with the player.
        bool hit = !(pr < it.x || pl > it.x + SIZE || pb < it.y || pt > it.y + SIZE);
        if (!hit)
            continue;

        it.active = false;   // eaten

        float s = player->GetScale();
        if (it.type == MUSHROOM) {
            s -= 0.5f;                   // shrink (Player::SetScale clamps to >= 1.0)
        }
        else { // TOMATO
            s += 0.5f;                   // grow (Player::SetScale clamps to <= 4.0)
        }
        player->SetScale(s);
    }
}

void ItemManager::render(Graphics* graphics, Camera* camera) {
    for (size_t i = 0; i < items.size(); i++) {
        Item& it = items[i];
        if (!it.active)
            continue;

        Sprite& s = (it.type == MUSHROOM) ? mushroomSprite : tomatoSprite;
        if (!s.IsValid())
            continue;

        // The camera scrolls the item with the world (was a manual it.x - camX).
        s.Draw(graphics, camera, D3DXVECTOR2(it.x, it.y));
    }
}

void ItemManager::reset() {
    for (size_t i = 0; i < items.size(); i++)
        items[i].active = true;
}
