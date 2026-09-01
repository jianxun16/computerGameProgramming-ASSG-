// author by limzhenxing

#include "BossState.h"
#include "Game.h"
#include "Graphics.h"
#include "InputManager.h"
#include "Background.h"
#include "TileMap.h"
#include "PlayerAnimation.h"
#include "PauseState.h"
#include "EndState.h"
#include "AudioManager.h"
#include "GameLog.h"

BossState::BossState(Game* game) : GameState(game)
{
    background = new Background();
    tileMap = new TileMap();
    player = new PlayerAnimation();
    bossTex = NULL;

    // World position of the boss placeholder (top-left). Sits on the right side
    // of the arena, roughly on the floor. Adjust to taste.
    bossX = 20.0f * TileMap::TILE;   // col 20
    bossY = 3.0f  * TileMap::TILE;   // near the floor (tweak for your art size)
}

BossState::~BossState()
{
    delete background;
    delete tileMap;
    delete player;
    if (bossTex != NULL) { bossTex->Release(); bossTex = NULL; }
}

void BossState::onEnter()
{
    Graphics* g = game->graphics();
    background->load(g->device());
    tileMap->load(g->device(), "Assets/Map/Map2.txt");   // the boss arena
    player->load(g->device(), g->width(), g->height());

    D3DXCreateTextureFromFile(g->device(), "Assets/Boss/67Boss.png", &bossTex);

    // Spawn the player on the arena floor at the entrance (left side).
    player->respawnToStart();
    player->setStartFeet(tileMap->groundTopYAt(player->getFeetWorldX()));
}

void BossState::update(InputManager* input)
{
    // Esc opens the pause menu (freezes the boss room while it sits on top).
    if (input->isKeyPressed(DIK_ESCAPE))
    {
        GameLog("Player paused the game (Esc)");
        game->pushState(new PauseState(game));
        return;
    }

    // Backspace leaves the boss room and returns to the level underneath.
    if (input->isKeyDown(DIK_BACK))
    {
        game->popState();
        return;
    }

    // Player walks + collides with the arena; boss is static for now.
    float deltaX = player->update(input, game->audio(), tileMap);
    background->update(deltaX);

    float hl, ht, hr, hb;
    player->getWorldHitbox(hl, ht, hr, hb);

    // Touched a spike in the arena -> game over (Lose menu).
    if (tileMap->rectSpike(hl, ht, hr, hb))
    {
        GameLog("Player hit a spike in the boss room -> Game Over");
        game->audio()->stopBGM();
        game->pushState(new EndState(game, EndState::RESULT_LOSE));
        return;
    }

    // Reached the boss -> victory (Win menu).
    // PLACEHOLDER win condition: for now simply walking up to the boss wins.
    // When the boss gets real health/attacks, replace this test with a proper
    // "boss defeated" check (e.g. bossHP <= 0).
    if (hr >= bossX)
    {
        GameLog("Player reached the boss -> Victory");
        game->audio()->stopBGM();
        game->pushState(new EndState(game, EndState::RESULT_WIN));
        return;
    }
}

void BossState::render(Graphics* gfx)
{
    float cameraX = player->getCameraX();

    background->render(gfx->sprite(), gfx->width());
    tileMap->render(gfx->sprite(), cameraX, gfx->width());

    // Draw the static boss, scrolled with the room (world -> screen).
    if (bossTex != NULL)
    {
        D3DXVECTOR3 pos(bossX - cameraX, bossY, 0.0f);
        gfx->sprite()->Draw(bossTex, NULL, NULL, &pos, D3DCOLOR_XRGB(255, 255, 255));
    }

    player->render(gfx->sprite());
}
