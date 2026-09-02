#include "PlayState.h"
#include "GameEngine.h" 
#include "PauseState.h"

void PlayState::Initialize(GameEngine* eng) {
    GameState::Initialize(eng);

    background.load(engine->GetGraphics());

    // 1. Use the dynamic mapFile string instead of hardcoding Map1
    map.load(engine->GetGraphics(), mapFile.c_str());

    player.Initialize(engine->GetGraphics(), D3DXVECTOR2(200.0f, 200.0f));

    if (!bgm.empty()) {
        engine->GetAudio()->Play(bgm, AudioManager::BGM);
    }
}

void PlayState::UpdateLogic(Input* input, float deltaTime) {
    if (input->IsKeyJustPressed(DIK_ESCAPE)) {
        engine->GetStateManager()->PushState(new PauseState());
        return;
    }

    // 2. Update the player and resolve collisions
    player.UpdateLogic(input, deltaTime, engine->GetAudio());
    player.ResolveMapCollisions(&map);

    // 3. Make the camera follow the player, clamping to the left edge
    D3DXVECTOR2 playerPos = player.GetPosition();
    float camX = playerPos.x - (engine->GetScreenWidth() / 2.0f);
    if (camX < 0.0f) camX = 0.0f;

    engine->GetCamera()->SetPosition(D3DXVECTOR2(camX, 0.0f));
    engine->GetCamera()->Update();
}

void PlayState::RenderFrame(Graphics* graphics) {
    background.render(graphics, engine->GetCamera());
    map.render(graphics, engine->GetCamera(), engine->GetScreenWidth());

    // 4. Actually draw the player
    player.RenderFrame(graphics, engine->GetCamera());
}