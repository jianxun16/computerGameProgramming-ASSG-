#include "MenuState.h"
#include "GameEngine.h"
#include "PlayState.h"
// #include "SettingState.h" // Uncomment when you migrate SettingState

void MenuState::Initialize(GameEngine* eng) {
    GameState::Initialize(eng);

    // Load textures using your central graphics wrapper
    background = engine->GetGraphics()->LoadTexture("Assets/menu/menu.png");
    buttonTex = engine->GetGraphics()->LoadTexture("Assets/menu/Button1.png");

    btnW = 260.0f;
    btnH = 72.0f;

    playBtn.label = "Play";
    settingBtn.label = "Setting";
    exitBtn.label = "Exit";

    // Initialize font globally for the menu state
    engine->GetGraphics()->LoadFont("Arial", 28, FW_BOLD);

    float W = (float)engine->GetScreenWidth();
    float H = (float)engine->GetScreenHeight();
    LayoutButtons(W, H);
}

void MenuState::LayoutButtons(float screenW, float screenH) {
    float gap = 26.0f; 
        float x = (screenW - btnW) / 2.0f;
        float y = screenH * 0.42f;

        Button* order[3] = { &playBtn, &settingBtn, &exitBtn }; 
        for (int i = 0; i < 3; i++) {
            order[i]->rect.left = (LONG)x; 
            order[i]->rect.top = (LONG)y; 
            order[i]->rect.right = (LONG)(x + btnW); 
            order[i]->rect.bottom = (LONG)(y + btnH); 
            y += btnH + gap; 
        }
}

bool MenuState::PointInRect(int px, int py, const RECT& r) {
    return px >= r.left && px <= r.right && py >= r.top && py <= r.bottom; 
}

void MenuState::UpdateLogic(Input* input, float deltaTime) {
    // Enter = Play shortcut[cite: 14]
    if (input->IsKeyJustPressed(DIK_RETURN)) {
        // Push the dynamic PlayState we built earlier
        engine->GetStateManager()->PushState(new PlayState("Assets/Map/Map1.txt", "bgm_stage1"));
        return;
    }

    int mx = input->GetMouseX();
    int my = input->GetMouseY();
    bool clicked = input->IsMouseButtonJustPressed(0);

    if (clicked) {
        if (PointInRect(mx, my, playBtn.rect)) {
            engine->GetStateManager()->PushState(new PlayState("Assets/Map/Map1.txt", "bgm_stage1"));
        }
        else if (PointInRect(mx, my, settingBtn.rect)) {
            // engine->GetStateManager()->PushState(new SettingState());
        }
        else if (PointInRect(mx, my, exitBtn.rect)) {
            PostQuitMessage(0);
        }
    }
}

void MenuState::DrawButton(Graphics* graphics, const Button& b, bool hovered) {
    if (buttonTex) {
        D3DSURFACE_DESC desc;
        buttonTex->GetLevelDesc(0, &desc); 

        float scaleX = btnW / (float)desc.Width; 
        float scaleY = btnH / (float)desc.Height; 

        D3DXMATRIX scaleMat, transMat, finalMat;
        D3DXMatrixScaling(&scaleMat, scaleX, scaleY, 1.0f);
        D3DXMatrixTranslation(&transMat, (float)b.rect.left, (float)b.rect.top, 0.0f);
        finalMat = scaleMat * transMat;

        // White when hovered, dimmed otherwise[cite: 14]
        D3DCOLOR tint = hovered ? D3DCOLOR_XRGB(255, 255, 255) : D3DCOLOR_XRGB(205, 205, 205);
        graphics->DrawSprite(buttonTex, NULL, &finalMat, tint);
    }

    RECT r = b.rect;
    graphics->DrawString(b.label, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 255));
}

void MenuState::RenderFrame(Graphics* graphics) {
    float W = (float)engine->GetScreenWidth();
    float H = (float)engine->GetScreenHeight();

    // 1. Draw Stretched Background
    if (background) {
        D3DSURFACE_DESC desc;
        background->GetLevelDesc(0, &desc);

        D3DXMATRIX scaleMat;
        D3DXMatrixScaling(&scaleMat, W / (float)desc.Width, H / (float)desc.Height, 1.0f);
        graphics->DrawSprite(background, NULL, &scaleMat, D3DCOLOR_XRGB(255, 255, 255));
    }

    // 2. Draw Title
    RECT tr = { 0, (LONG)(H * 0.14f), (LONG)W, (LONG)(H * 0.30f) };
    graphics->DrawString("defeat the 67 boss", &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP, D3DCOLOR_XRGB(255, 240, 90));

    // 3. Draw Buttons
    int mx = engine->GetInput()->GetMouseX();
    int my = engine->GetInput()->GetMouseY();

    DrawButton(graphics, playBtn, PointInRect(mx, my, playBtn.rect));
    DrawButton(graphics, settingBtn, PointInRect(mx, my, settingBtn.rect));
    DrawButton(graphics, exitBtn, PointInRect(mx, my, exitBtn.rect));
}