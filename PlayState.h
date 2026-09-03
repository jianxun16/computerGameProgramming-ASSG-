#pragma once
#include "GameState.h"
#include "StateManager.h"
#include "TileMap.h"
#include "Background.h"
#include "Camera.h"
#include "Player.h"
#include "ItemManager.h"

class PlayState : public GameState {
private:
    TileMap map;
    Background background;
    string mapFile;
    string bgm;
    Player player;
    ItemManager items;

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