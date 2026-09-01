// author by limzhenxing

#pragma once
#include "GameState.h"
#include <d3d9.h>
#include <d3dx9.h>

class Background;
class PlayerAnimation;
class TileMap;

// Boss room: a closed arena (Map2.txt) entered from the end of PlayState. The
// boss is a static placeholder (67Boss.png) for now. Backspace pops back.
class BossState : public GameState
{
public:
    BossState(Game* game);
    ~BossState();

    void onEnter() override;
    void update(InputManager* input) override;
    void render(Graphics* gfx) override;

private:
    Background*      background;
    TileMap*         tileMap;
    PlayerAnimation* player;

    // Static boss placeholder; bossX / bossY are its world position (top-left).
    LPDIRECT3DTEXTURE9 bossTex;
    float bossX;
    float bossY;
};
