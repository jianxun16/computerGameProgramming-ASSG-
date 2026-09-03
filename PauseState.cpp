#include "PauseState.h"
#include "GameEngine.h"
#include <cstdio>

void PauseState::Initialize(GameEngine* eng) {
    GameState::Initialize(eng);

    float W = (float)engine->GetScreenWidth();
    float H = (float)engine->GetScreenHeight();

    float panelW = 440.0f, panelH = 380.0f;
    float panelX = (W - panelW) / 2.0f;
    float panelY = (H - panelH) / 2.0f;
    float inset = 40.0f;
    float trackX = panelX + inset;
    float trackW = panelW - inset * 2.0f;

    // Setup BGM Slider
    bgm.x = trackX; bgm.y = panelY + 112.0f; bgm.w = trackW; bgm.h = 12.0f;
    bgm.dragging = false;
    bgm.value = engine->GetAudio()->GetMusicVolume();

    // Setup SFX Slider
    sfx.x = trackX; sfx.y = panelY + 182.0f; sfx.w = trackW; sfx.h = 12.0f;
    sfx.dragging = false;
    sfx.value = engine->GetAudio()->GetSFXVolume();

    continueW = trackW; continueH = 44.0f;
    continueX = trackX; continueY = panelY + 230.0f;

    menuW = trackW; menuH = 44.0f;
    menuX = trackX; menuY = panelY + 288.0f;

    // Ensure font is loaded
    engine->GetGraphics()->LoadFont("Arial", 22, FW_BOLD);
}

bool PauseState::PointInRect(int px, int py, float x, float y, float w, float h) {
    return px >= x && px <= x + w && py >= y && py <= y + h;
}

bool PauseState::NearTrack(int px, int py, const Slider& s) {
    float grab = 16.0f;
    return px >= s.x - grab && px <= s.x + s.w + grab && py >= s.y - grab && py <= s.y + s.h + grab;
}

void PauseState::UpdateLogic(Input* input, float deltaTime) {
    if (input->IsKeyJustPressed(DIK_ESCAPE)) {
        engine->GetStateManager()->PopState();
        return;
    }

    int mx = input->GetMouseX();
    int my = input->GetMouseY();
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

        // BGM Logic
        if (bgm.dragging) {
            float v = (mx - bgm.x) / bgm.w;
            if (v < 0.0f) v = 0.0f; if (v > 1.0f) v = 1.0f;
            bgm.value = v;
            engine->GetAudio()->SetMusicVolume(v);
        }

        // SFX Logic
        if (sfx.dragging) {
            float v = (mx - sfx.x) / sfx.w;
            if (v < 0.0f) v = 0.0f; if (v > 1.0f) v = 1.0f;
            sfx.value = v;
            engine->GetAudio()->SetSFXVolume(v);
        }
    }

    if (clicked) {
        if (PointInRect(mx, my, continueX, continueY, continueW, continueH)) {
            engine->GetStateManager()->PopState();
        }
        else if (PointInRect(mx, my, menuX, menuY, menuW, menuH)) {
            engine->GetAudio()->StopBGM();
            engine->GetStateManager()->PopToBottom();   // clear the game -> back to menu
        }
    }
}

void PauseState::RenderFrame(Graphics* graphics) {
    float W = (float)engine->GetScreenWidth();
    float H = (float)engine->GetScreenHeight();

    float panelW = 440.0f, panelH = 380.0f;
    float panelX = (W - panelW) / 2.0f;
    float panelY = (H - panelH) / 2.0f;

    // Dim the screen behind the pause menu
    graphics->DrawRect(0.0f, 0.0f, W, H, D3DCOLOR_ARGB(150, 0, 0, 0));

    // Main Panel
    graphics->DrawRect(panelX, panelY, panelW, panelH, D3DCOLOR_ARGB(235, 28, 32, 46));

    // Hover Logic for Buttons
    int mx = engine->GetInput()->GetMouseX();
    int my = engine->GetInput()->GetMouseY();
    bool hovContinue = PointInRect(mx, my, continueX, continueY, continueW, continueH);
    bool hovMenu = PointInRect(mx, my, menuX, menuY, menuW, menuH);

    // Buttons
    graphics->DrawRect(continueX, continueY, continueW, continueH,
        hovContinue ? D3DCOLOR_ARGB(255, 90, 160, 115) : D3DCOLOR_ARGB(255, 60, 130, 85));
    graphics->DrawRect(menuX, menuY, menuW, menuH,
        hovMenu ? D3DCOLOR_ARGB(255, 200, 90, 90) : D3DCOLOR_ARGB(255, 130, 60, 60));

    // BGM Slider Bar
    graphics->DrawRect(bgm.x, bgm.y, bgm.w, bgm.h, D3DCOLOR_ARGB(255, 70, 74, 90)); // Track
    graphics->DrawRect(bgm.x, bgm.y, bgm.value * bgm.w, bgm.h, D3DCOLOR_ARGB(255, 90, 200, 120)); // Fill
    graphics->DrawRect(bgm.x + (bgm.value * bgm.w) - 7.0f, bgm.y - 6.0f, 14.0f, bgm.h + 12.0f, D3DCOLOR_ARGB(255, 240, 240, 245)); // Knob

    // SFX Slider Bar
    graphics->DrawRect(sfx.x, sfx.y, sfx.w, sfx.h, D3DCOLOR_ARGB(255, 70, 74, 90)); // Track
    graphics->DrawRect(sfx.x, sfx.y, sfx.value * sfx.w, sfx.h, D3DCOLOR_ARGB(255, 90, 200, 120)); // Fill
    graphics->DrawRect(sfx.x + (sfx.value * sfx.w) - 7.0f, sfx.y - 6.0f, 14.0f, sfx.h + 12.0f, D3DCOLOR_ARGB(255, 240, 240, 245)); // Knob

    // Text Rendering
    char buf[64];

    RECT titleR = { (LONG)panelX, (LONG)(panelY + 24.0f), (LONG)(panelX + panelW), (LONG)(panelY + 74.0f) };
    graphics->DrawString("PAUSED", &titleR, DT_CENTER | DT_VCENTER | DT_SINGLELINE, D3DCOLOR_ARGB(255, 240, 240, 245));

    sprintf_s(buf, "Music   %d%%", (int)(bgm.value * 100.0f + 0.5f));
    RECT bgmR = { (LONG)bgm.x, (LONG)(bgm.y - 34.0f), (LONG)(bgm.x + bgm.w), (LONG)(bgm.y - 8.0f) };
    graphics->DrawString(buf, &bgmR, DT_LEFT | DT_VCENTER | DT_SINGLELINE, D3DCOLOR_ARGB(255, 220, 220, 228));

    sprintf_s(buf, "Sound   %d%%", (int)(sfx.value * 100.0f + 0.5f));
    RECT sfxR = { (LONG)sfx.x, (LONG)(sfx.y - 34.0f), (LONG)(sfx.x + sfx.w), (LONG)(sfx.y - 8.0f) };
    graphics->DrawString(buf, &sfxR, DT_LEFT | DT_VCENTER | DT_SINGLELINE, D3DCOLOR_ARGB(255, 220, 220, 228));

    RECT contR = { (LONG)continueX, (LONG)continueY, (LONG)(continueX + continueW), (LONG)(continueY + continueH) };
    graphics->DrawString("Resume Game", &contR, DT_CENTER | DT_VCENTER | DT_SINGLELINE, D3DCOLOR_ARGB(255, 255, 255, 255));

    RECT menuR = { (LONG)menuX, (LONG)menuY, (LONG)(menuX + menuW), (LONG)(menuY + menuH) };
    graphics->DrawString("Main Menu", &menuR, DT_CENTER | DT_VCENTER | DT_SINGLELINE, D3DCOLOR_ARGB(255, 255, 255, 255));
}