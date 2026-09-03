#include "EndState.h"
#include "GameEngine.h"
#include "MenuState.h"
#include "GameLog.h"

void EndState::Initialize(GameEngine* eng) {
    GameState::Initialize(eng);

    // A crisp font for the title / button label.
    engine->GetGraphics()->LoadFont("Arial", 28, FW_BOLD);

    float W = (float)engine->GetScreenWidth();
    float H = (float)engine->GetScreenHeight();

    // Centred panel: title on top, one button near the bottom.
    float panelW = 440.0f, panelH = 260.0f;
    float panelX = (W - panelW) / 2.0f;
    float panelY = (H - panelH) / 2.0f;

    float inset = 40.0f;
    menuW = panelW - inset * 2.0f;
    menuH = 48.0f;
    menuX = panelX + inset;
    menuY = panelY + panelH - inset - menuH;
}

bool EndState::PointInRect(int px, int py, float x, float y, float w, float h) {
    return px >= x && px <= x + w && py >= y && py <= y + h;
}

void EndState::UpdateLogic(Input* input, float deltaTime) {
    int mx = input->GetMouseX();
    int my = input->GetMouseY();
    bool clicked = input->IsMouseButtonJustPressed(0);

    // Esc or the button clear the whole game stack back to the menu.
    if (input->IsKeyJustPressed(DIK_ESCAPE) ||
        (clicked && PointInRect(mx, my, menuX, menuY, menuW, menuH))) {
        GameLog("Player returned to Menu (game cleared)");
        engine->GetAudio()->StopBGM();
        engine->GetStateManager()->PopToBottom();
        return;
    }
}

void EndState::RenderFrame(Graphics* graphics) {
    float W = (float)engine->GetScreenWidth();
    float H = (float)engine->GetScreenHeight();

    float panelW = 440.0f, panelH = 260.0f;
    float panelX = (W - panelW) / 2.0f;
    float panelY = (H - panelH) / 2.0f;

    bool win = (result == RESULT_WIN);
    const char* title = win ? "VICTORY" : "GAME OVER";
    D3DCOLOR titleColor = win ? D3DCOLOR_ARGB(255, 120, 220, 140)
                              : D3DCOLOR_ARGB(255, 220, 90, 90);

    // Dim the frozen scene, then the panel.
    graphics->DrawRect(0.0f, 0.0f, W, H, D3DCOLOR_ARGB(160, 0, 0, 0));
    graphics->DrawRect(panelX, panelY, panelW, panelH, D3DCOLOR_ARGB(235, 28, 32, 46));

    // Button (lighter when hovered).
    int mx = engine->GetInput()->GetMouseX();
    int my = engine->GetInput()->GetMouseY();
    bool hov = PointInRect(mx, my, menuX, menuY, menuW, menuH);
    graphics->DrawRect(menuX, menuY, menuW, menuH,
                       hov ? D3DCOLOR_ARGB(255, 90, 140, 210) : D3DCOLOR_ARGB(255, 60, 95, 150));

    // Title text.
    RECT tr = { (LONG)panelX, (LONG)(panelY + 40.0f), (LONG)(panelX + panelW), (LONG)(panelY + 120.0f) };
    graphics->DrawString(title, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP, titleColor);

    // Button label.
    RECT br = { (LONG)menuX, (LONG)menuY, (LONG)(menuX + menuW), (LONG)(menuY + menuH) };
    graphics->DrawString("Back to Menu", &br, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP,
                         D3DCOLOR_ARGB(255, 255, 255, 255));
}
