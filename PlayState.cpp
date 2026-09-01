#include "PlayState.h"
#include "Game.h"
#include "Graphics.h"
#include "InputManager.h"
#include "Background.h"
#include "TileMap.h"
#include "ItemManager.h"
#include "PlayerAnimation.h"
#include "BossState.h"
#include "PauseState.h"
#include "EndState.h"
#include "AudioManager.h"
#include "GameLog.h"

// A tiny 2x2 translucent red texture we stretch over the screen on game over.
static LPDIRECT3DTEXTURE9 MakeRedOverlay(IDirect3DDevice9* device)
{
    LPDIRECT3DTEXTURE9 tex = NULL;
    if (FAILED(device->CreateTexture(2, 2, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex, NULL)))
        return NULL;

    D3DLOCKED_RECT lr;
    tex->LockRect(0, &lr, NULL, 0);
    DWORD* px = (DWORD*)lr.pBits;
    for (int i = 0; i < 4; i++)
        px[i] = D3DCOLOR_ARGB(120, 200, 30, 30);   // semi-transparent red
    tex->UnlockRect(0);
    return tex;
}

PlayState::PlayState(Game* game) : GameState(game)
{
    background = new Background();
    tileMap = new TileMap();
    items = new ItemManager();
    player = new PlayerAnimation();
    gameOver = false;
    enteredBoss = false;
    overlayTex = NULL;
}

PlayState::~PlayState()
{
    delete background;
    delete tileMap;
    delete items;
    delete player;
    if (overlayTex != NULL) { overlayTex->Release(); overlayTex = NULL; }
}

void PlayState::onEnter()
{
    Graphics* g = game->graphics();
    background->load(g->device());
    tileMap->load(g->device());
    items->load(g->device());
    player->load(g->device(), g->width(), g->height());
    overlayTex = MakeRedOverlay(g->device());

    // Drop the player onto whatever ground is under the spawn column.
    player->setStartFeet(tileMap->groundTopYAt(player->getFeetWorldX()));

    // Level music fades in as the level begins.
    game->audio()->playMap1BGM();
    game->audio()->fadeInBGM(1.5f);
}

void PlayState::onResume()
{
    // Coming back from the pause menu or the boss room: bring the level music
    // back up (it was faded out when we walked into the boss room).
    game->audio()->fadeInBGM(1.5f);
}

void PlayState::restart()
{
    player->respawnToStart();
    player->setStartFeet(tileMap->groundTopYAt(player->getFeetWorldX()));
    items->reset();
    gameOver = false;
    enteredBoss = false;
}

void PlayState::update(InputManager* input)
{
    // Esc opens the pause menu (freezes the game while it sits on top).
    if (input->isKeyPressed(DIK_ESCAPE))
    {
        GameLog("Player paused the game (Esc)");
        game->pushState(new PauseState(game));
        return;
    }

    // ----- Game over: the Lose menu is on top now; nothing to do here. -----
    if (gameOver)
        return;

    // Backspace pops back to the menu (still on the stack underneath).
    if (input->isKeyDown(DIK_BACK))
    {
        game->popState();
        return;
    }

    // Player moves + collides with the tile map; its movement scrolls the world.
    float deltaX = player->update(input, game->audio(), tileMap);
    background->update(deltaX);
    items->update(player);

    // Fell into the bottomless pit, or touched a spike -> game over.
    float hl, ht, hr, hb;
    player->getWorldHitbox(hl, ht, hr, hb);
    bool fell   = player->getFeetY() > TileMap::ROWS * TileMap::TILE;
    bool spiked = tileMap->rectSpike(hl, ht, hr, hb);
    if (fell || spiked)
    {
        gameOver = true;   // freezes this state; the Lose menu overlay takes over
        GameLog(fell ? "Player fell into the pit -> Game Over"
                     : "Player hit a spike -> Game Over");
        game->audio()->stopBGM();
        game->pushState(new EndState(game, EndState::RESULT_LOSE));
        return;
    }

    // Reached the end of the level -> walk into the boss room (push it on top).
    // Fires once so we don't re-enter every frame while standing at the edge.
    const float LEVEL_END_X = 26.0f * TileMap::TILE;
    if (!enteredBoss && player->getFeetWorldX() >= LEVEL_END_X)
    {
        enteredBoss = true;
        GameLog("Player reached the level end -> entering Boss room");
        game->audio()->fadeOutBGM(1.0f);   // fade Map1 music out on the way to the next level
        game->pushState(new BossState(game));
    }
}

void PlayState::render(Graphics* gfx)
{
    float cameraX = player->getCameraX();

    background->render(gfx->sprite(), gfx->width());
    tileMap->render(gfx->sprite(), cameraX, gfx->width());
    items->render(gfx->sprite(), cameraX);
    player->render(gfx->sprite());

    // Red tint over the whole screen when the game is over.
    if (gameOver && overlayTex != NULL)
    {
        D3DXMATRIX scaleM, world, identity;
        D3DXMatrixScaling(&scaleM, gfx->width() / 2.0f, gfx->height() / 2.0f, 1.0f);
        gfx->sprite()->SetTransform(&scaleM);

        D3DXVECTOR3 zero(0.0f, 0.0f, 0.0f);
        gfx->sprite()->Draw(overlayTex, NULL, NULL, &zero, D3DCOLOR_ARGB(255, 255, 255, 255));

        D3DXMatrixIdentity(&identity);
        gfx->sprite()->SetTransform(&identity);
    }
}
