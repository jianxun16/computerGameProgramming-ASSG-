// author by limzhenxing

#pragma once
#include "GameState.h"
#include <d3d9.h>
#include <d3dx9.h>

class Background;
class PlayerAnimation;
class TileMap;

// The boss room: a closed arena (Map2.txt) the player walks into from the end
// of PlayState. For now the boss itself is just a STATIC placeholder sprite
// (67Boss.png) - no health / attacks yet. Backspace pops back to the level.
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

    // Static boss placeholder. bossX / bossY are its WORLD position (top-left);
    // it scrolls with the room like any tile. Tweak them to move the boss.
    LPDIRECT3DTEXTURE9 bossTex;
    float bossX;
    float bossY;
};
