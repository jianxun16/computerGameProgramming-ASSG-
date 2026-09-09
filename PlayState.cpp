#include "PlayState.h"
#include "GameEngine.h"
#include "PauseState.h"
#include "BossState.h"
#include "EndState.h"
#include "GameLog.h"

void PlayState::Initialize(GameEngine* eng) {
    GameState::Initialize(eng);

    background.load(engine->GetGraphics());

    // 1. Use the dynamic mapFile string instead of hardcoding Map1
    map.load(engine->GetGraphics(), mapFile.c_str());

    player.Initialize(engine->GetGraphics(), D3DXVECTOR2(300.0f, 400.0f));

    // Items now live here (was ItemManager). Each sits on the floor (floor top =
    // row 7). The ONLY thing that makes a mushroom differ from a tomato is the
    // effect we inject here -- the Item class itself is generic.
    Graphics* gfx = engine->GetGraphics();
    float onFloor = 7.0f * TileMap::TILE - 64;   // 64px item resting on the floor

    tomato.sprite.SetTexture(gfx->LoadTexture("Assets/item/tomato.png"));
    tomato.x = 11.0f * TileMap::TILE;
    tomato.y = onFloor;
    tomato.active = true;
    tomato.effect = [](Player& p) { p.SetScale(p.GetScale() + 0.5f); };   // grow

    mushroom.sprite.SetTexture(gfx->LoadTexture("Assets/item/mushroom.png"));
    mushroom.x = 20.0f * TileMap::TILE;
    mushroom.y = onFloor;
    mushroom.active = true;
    mushroom.effect = [](Player& p) { p.SetScale(p.GetScale() - 0.5f); };  // shrink

    enteredBoss = false;

    if (!bgm.empty()) {
        engine->GetAudio()->Play(bgm, AudioManager::BGM);
    }
}

void PlayState::Resume() {
    // Coming back from the pause menu (or the boss room): bring the level music back.
    if (!bgm.empty()) {
        engine->GetAudio()->Play(bgm, AudioManager::BGM);
    }
}

void PlayState::UpdateLogic(Input* input, float deltaTime) {
    // Esc opens the pause menu (freezes the level).
    if (input->IsKeyJustPressed(DIK_ESCAPE)) {
        GameLog("Player paused the game (Esc)");
        engine->GetStateManager()->PushState(new PauseState());
        return;
    }

    // Backspace pops back to the menu underneath.
    if (input->IsKeyJustPressed(DIK_BACK)) {
        engine->GetStateManager()->PopState();
        return;
    }

    // 2. Update the player and resolve collisions
    player.UpdateLogic(input, deltaTime, engine->GetAudio());
    player.ResolveMapCollisions(&map);

    // Pick up any item the player overlaps. Each item just runs the effect that
    // Initialize injected -- no mushroom/tomato check here.
    float pl, pt, pr, pb;
    player.GetWorldHitbox(pl, pt, pr, pb);
    if (mushroom.active && mushroom.overlaps(pl, pt, pr, pb)) {
        mushroom.applyEffect(player);
        mushroom.active = false;
    }
    if (tomato.active && tomato.overlaps(pl, pt, pr, pb)) {
        tomato.applyEffect(player);
        tomato.active = false;
    }

    // 3. Make the camera follow the player, clamping to the left edge
    D3DXVECTOR2 playerPos = player.GetPosition();
    float camX = playerPos.x - (engine->GetScreenWidth() / 2.0f);
    if (camX < 0.0f) camX = 0.0f;

    engine->GetCamera()->SetPosition(D3DXVECTOR2(camX, 0.0f));
    engine->GetCamera()->Update();

    // 4. Fell into the pit or walked into a spike -> Game Over (god mode ignores both).
    float hl, ht, hr, hb;
    player.GetWorldHitbox(hl, ht, hr, hb);
    bool fell = player.GetFeetY() > TileMap::ROWS * TileMap::TILE;
    bool spiked = map.rectSpike(hl, ht, hr, hb);
    if ((fell || spiked) && !player.IsGodMode()) {
        GameLog(fell ? "Player fell into the pit -> Game Over"
                     : "Player hit a spike -> Game Over");
        engine->GetAudio()->StopBGM();
        engine->GetStateManager()->PushState(new EndState(EndState::RESULT_LOSE));
        return;
    }

    // 5. Reached the level end -> push the boss room. Fires once (enteredBoss).
    const float LEVEL_END_X = 26.0f * TileMap::TILE;
    if (!enteredBoss && player.GetFeetWorldX() >= LEVEL_END_X) {
        enteredBoss = true;
        GameLog("Player reached the level end -> entering Boss room");
        engine->GetAudio()->StopBGM();
        engine->GetStateManager()->PushState(new BossState());
    }
}

void PlayState::RenderFrame(Graphics* graphics) {
    background.render(graphics, engine->GetCamera());
    map.render(graphics, engine->GetCamera(), engine->GetScreenWidth());

    // Draw the items, then the player
    mushroom.render(graphics, engine->GetCamera());
    tomato.render(graphics, engine->GetCamera());
    player.RenderFrame(graphics, engine->GetCamera());
}
