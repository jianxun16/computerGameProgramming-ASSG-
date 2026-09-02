#include "PlayState.h"
#include "GameEngine.h" 

void PlayState::Initialize(GameEngine* eng) {
    GameState::Initialize(eng); 

    // load assets from engine
    background.load(engine->GetGraphics());
    map.load(engine->GetGraphics(), "Assets/Map/Map1.txt");
    // load player
    player.Initialize(engine->GetGraphics(), D3DXVECTOR2(200.0f, 200.0f));

    if (!bgm.empty()) {
        engine->GetAudio()->Play(bgm);
    }
}

void PlayState::UpdateLogic(Input* input, float deltaTime) {
    // 1. Pause Menu check
    if (input->IsKeyJustPressed(DIK_ESCAPE)) {
        engine->GetStateManager()->PushState(new PauseState());
        return;
    }

    // 2. Placeholder Camera Scroll (To test your background loop)
    engine->GetCamera()->Move(D3DXVECTOR2(100.0f * deltaTime, 0.0f));
    engine->GetCamera()->Update();
}

void PlayState::RenderFrame(Graphics* graphics) {
    // Draw back-to-front
    background.render(graphics, engine->GetCamera());

    // Pass the engine's screen width so the map only draws visible tiles
    map.render(graphics, engine->GetCamera(), engine->GetScreenWidth());
}