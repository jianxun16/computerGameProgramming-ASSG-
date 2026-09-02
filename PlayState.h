#pragma once
#include "GameState.h"
#include "StateManager.h"
#include "TileMap.h"
#include "Background.h"
#include "Camera.h"
#include "Player.h" 

class PlayState : public GameState {
private:
    TileMap map;
    Background background;
    string mapFile;
    string bgm;
    Player player;

public:
    PlayState(string mapPath, string bgmName) {
        mapFile = mapPath;
        bgm = bgmName;
    }

    void Initialize(GameEngine* eng) override;
    void UpdateLogic(Input* input, float deltaTime) override;
    void RenderFrame(Graphics* graphics) override;
};