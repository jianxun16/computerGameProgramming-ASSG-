#include "PauseState.h"
#include "GameEngine.h"
// #include "MenuState.h" // For when the player clicks "Back to Menu"

void PauseState::Initialize(GameEngine* eng) {
    GameState::Initialize(eng);

    uiTex = engine->GetGraphics()->LoadTexture("Assets/menu/ui_box.png");

    float W = (float)engine->GetScreenWidth();
    float H = (float)engine->GetScreenHeight();

    float panelW = 440.0f, panelH = 380.0f;
    float panelX = (W - panelW) / 2.0f;
    float panelY = (H - panelH) / 2.0f;
    float inset = 40.0f;
    float trackX = panelX + inset;
    float trackW = panelW - inset * 2.0f;

    // Setup Sliders
    bgm.x = trackX; bgm.y = panelY + 112.0f; bgm.w = trackW; bgm.h = 12.0f;
    bgm.dragging = false;
    engine->GetAudio()->GetMusicVolume();

    sfx.x = trackX; sfx.y = panelY + 182.0f; sfx.w = trackW; sfx.h = 12.0f;
    sfx.dragging = false;
    engine->GetAudio()->GetSFXVolume();

    continueX = trackX; continueY = panelY + 230.0f; continueW = trackW; continueH = 44.0f;
    menuX = trackX; menuY = panelY + 288.0f; menuW = trackW; menuH = 44.0f;
}

bool PauseState::PointInRect(int px, int py, float x, float y, float w, float h) {
    return px >= x && px <= x + w && py >= y && py <= y + h;
}

bool PauseState::NearTrack(int px, int py, const Slider& s) {
    float grab = 16.0f;
    return px >= s.x - grab && px <= s.x + s.w + grab && py >= s.y - grab && py <= s.y + s.h + grab;
}

void PauseState::UpdateLogic(Input* input, float deltaTime) {
    // unpause with esc
    if (input->IsKeyJustPressed(DIK_ESCAPE)) {
        engine->GetStateManager()->PopState();
        return;
    }

    int mx = input->GetMouseDX(); // Assuming these exist in your Input wrapper
    int my = input->GetMouseDY();
    bool down = input->IsMouseButtonDown(0);
    bool clicked = input->IsMouseButtonJustPressed(0);

    if (!down) {
        bgm.dragging = false;
        sfx.dragging = false;
    }
    else {
        if (clicked) {
            if (NearTrack(mx, my, bgm)) bgm.dragging = true;
            else if (NearTrack(mx, my, sfx)) sfx.dragging = true;
        }

        if (bgm.dragging) {
            float v = (mx - bgm.x) / bgm.w;
            if (v < 0.0f) v = 0.0f; if (v > 1.0f) v = 1.0f;
            bgm.value = v;
            // engine->GetAudio()->SetMusicVolume(v);
        }
        // ... (Repeat exact same logic for SFX slider)
    }

    if (clicked) {
        if (PointInRect(mx, my, continueX, continueY, continueW, continueH)) {
            engine->GetStateManager()->PopState();
        }
        else if (PointInRect(mx, my, menuX, menuY, menuW, menuH)) {
            // engine->GetAudio()->StopBGM();
            // engine->GetStateManager()->ChangeState(new MenuState()); // Clears stack and loads menu
        }
    }
}

// Stretches your UI texture to form the panels and buttons
void PauseState::DrawUIBox(Graphics* graphics, float x, float y, float w, float h) {
    D3DXMATRIX scaleMat, transMat, finalMat;

    // Assuming your UI texture is 10x10 pixels, we scale it to match W and H
    D3DXMatrixScaling(&scaleMat, w / 10.0f, h / 10.0f, 1.0f);
    D3DXMatrixTranslation(&transMat, x, y, 0.0f);
    finalMat = scaleMat * transMat;

    graphics->DrawSprite(uiTex, NULL, &finalMat);
}

void PauseState::RenderFrame(Graphics* graphics) {

    // panel
    float W = (float)engine->GetScreenWidth();
    float H = (float)engine->GetScreenHeight();
    DrawUIBox(graphics, (W - 440.0f) / 2.0f, (H - 380.0f) / 2.0f, 440.0f, 380.0f);

    // buttons
    DrawUIBox(graphics, continueX, continueY, continueW, continueH);
    DrawUIBox(graphics, menuX, menuY, menuW, menuH);

    // sliders
    DrawUIBox(graphics, bgm.x, bgm.y, bgm.w, bgm.h); 
    DrawUIBox(graphics, bgm.x + (bgm.value * bgm.w) - 7.0f, bgm.y - 6.0f, 14.0f, bgm.h + 12.0f);

    // (Repeat slider drawing for SFX)
}