#pragma once
#include "GameState.h"
#include <d3d9.h>
#include <d3dx9.h>

class Background;
class PlayerAnimation;
class TileMap;
class ItemManager;

// The actual gameplay: parallax background + tile-map floor + items + the
// playable warrior. Fall into the bottomless pit and it's game over
// (press R to restart). Backspace pops back to the menu.
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
    bool               enteredBoss;  // true once we've pushed the boss room (fire once)
    LPDIRECT3DTEXTURE9 overlayTex;   // red tint shown on game over
};
