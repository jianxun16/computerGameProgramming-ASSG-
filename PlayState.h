#pragma once
#include "GameState.h"
#include "StateManager.h"
#include "TileMap.h"
#include "Background.h"
#include "Camera.h"
#include "Player.h"
#include "Sprite.h"
#include <functional>
#include <vector>

class PlayState : public GameState {
private:
    TileMap map;
    Background background;
    string mapFile;
    string bgm;
    // A collectable item, owned directly by the PlayState (was ItemManager).
    // It does NOT know what it does -- the effect is a function injected in
    // Initialize, and applyEffect() runs it when the player picks the item up.
    // So the same Item type can be a mushroom, a tomato, or anything else.
    struct Item {
        Sprite sprite;                         // renderer for this item's texture
        float  x = 0.0f, y = 0.0f;             // top-left in world coords
        int    size = 64;                      // the 64x64 item cell
        bool   active = false;
        std::function<void(Player&)> effect;   // "what eating me does" (set in Initialize)

        void ApplyEffect(Player& player) { if (effect) effect(player); }

        bool Overlaps(float pl, float pt, float pr, float pb) const {
            return !(pr < x || pl > x + size || pb < y || pt > y + size);
        }

        void render(Graphics* graphics, Camera* camera) {
            if (active && sprite.IsValid())
                sprite.Draw(graphics, camera, D3DXVECTOR2(x, y));
        }
    };

    Player player;
    vector<Item> itemList;

    bool enteredBoss;   // true once the boss room has been pushed (fires once)

public:
    PlayState(string mapPath, string bgmName) {
        mapFile = mapPath;
        bgm = bgmName;
        enteredBoss = false;
    }

    void Initialize(GameEngine* eng) override;
    void Resume() override;
    void UpdateLogic(Input* input, float deltaTime) override;
    void RenderFrame(Graphics* graphics) override;
};