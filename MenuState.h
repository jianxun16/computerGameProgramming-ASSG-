#pragma once
#include "GameState.h"
#include "Sprite.h"
#include <d3dx9.h>

class MenuState : public GameState {
private:
    struct Button {
        RECT rect;
        const char* label;
    };

    LPDIRECT3DTEXTURE9 background;
    LPDIRECT3DTEXTURE9 buttonTex;

    Sprite bgSprite;       // reusable renderers for the menu UI
    Sprite buttonSprite;

    Button playBtn;
    Button settingBtn;
    Button exitBtn;

    float btnW;
    float btnH;

    void LayoutButtons(float screenW, float screenH);
    bool PointInRect(int px, int py, const RECT& r);
    void DrawButton(Graphics* graphics, const Button& b, bool hovered);

public:
    void Initialize(GameEngine* eng) override;
    void UpdateLogic(Input* input, float deltaTime) override;
    void RenderFrame(Graphics* graphics) override;
};