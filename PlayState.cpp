#include "PlayState.h"
#include "GameEngine.h"
#include "PauseState.h"
#include "BossState.h"
#include "EndState.h"
#include "GameLog.h"

void PlayState::Initialize(GameEngine* eng) {
    GameState::Initialize(eng);

    background.Load(engine->GetGraphics());

    map.load(engine->GetGraphics(), mapFile.c_str());

    player.Initialize(engine->GetGraphics(), D3DXVECTOR2(300.0f, 400.0f));

    Graphics* gfx = engine->GetGraphics();
    float onFloor = 7.0f * TileMap::TILE - 64;   // 64px item resting on the floor
    itemList.clear();

    Item tomato;
    tomato.sprite.SetTexture(engine->GetGraphics()->LoadTexture("Assets/item/tomato.png"));
    tomato.x = 11.0f * TileMap::TILE;
    tomato.y = onFloor;
    tomato.active = true;
    tomato.effect = [](Player& p) { p.SetScale(p.GetScale() + 0.5f); };   // grow
    itemList.push_back(tomato);

    Item mushroom;
    mushroom.sprite.SetTexture(engine->GetGraphics()->LoadTexture("Assets/item/mushroom.png"));
    mushroom.x = 20.0f * TileMap::TILE;
    mushroom.y = onFloor;
    mushroom.active = true;
    mushroom.effect = [](Player& p) { p.SetScale(p.GetScale() - 0.5f); };  // shrink
    itemList.push_back(mushroom);

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
    for (auto& item : itemList) {
        if (item.active && item.Overlaps(pl, pt, pr, pb)) {
            item.ApplyEffect(player);
            item.active = false;
        }
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
    background.Render(graphics, engine->GetCamera());
    map.render(graphics, engine->GetCamera(), engine->GetScreenWidth());

    // Draw the items, then the player
    for (auto& item : itemList) {
        item.render(graphics, engine->GetCamera());
    }
    player.RenderFrame(graphics, engine->GetCamera());
}
