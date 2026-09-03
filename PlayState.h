#pragma once
#include "GameState.h"
#include <d3d9.h>
#include <d3dx9.h>

class Background;
class PlayerAnimation;
class TileMap;
class ItemManager;

// Gameplay: parallax background + tile-map floor + items + the playable
// warrior. Fall into the pit or hit a spike = game over. Backspace pops to menu.
class PlayState : public GameState
{
public:
    PlayState(Game* game);
    ~PlayState();

    void onEnter() override;
    void onResume() override;
    void update(InputManager* input) override;
    void render(Graphics* gfx) override;

private:
    void restart();

    Background*      background;
    TileMap*         tileMap;
    ItemManager*     items;
    PlayerAnimation* player;

    bool               gameOver;
    bool               enteredBoss;  // true once the boss room is pushed
    LPDIRECT3DTEXTURE9 overlayTex;   // red game-over tint
};
